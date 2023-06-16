#include "decoder.h"

Decoder::Decoder()
{
    init();
}

bool Decoder::decode(const uint8_t *src, const std::size_t srclen, Http2_Stream& incstream)
{
    init();

    std::size_t cur=0;

//    std::cout<<srclen<<"\n";

    while(cur<srclen){

        //解码状态机
        switch (state) {
        case decodeState::OPCODE:{
            const uint8_t c=src[cur];


            if((c&0xe0)==0x20){
                opcode=decodeOpcode::INDEXED;
                state=decodeState::READ_TABLE_SIZE;

            }else if(c&0x80){
                opcode=decodeOpcode::INDEXED;
                state=decodeState::READ_INDEX;

            }else{
                if (0 == c || 0x40 == c || 0x10 == c) {
                    opcode = decodeOpcode::NOT_INDEXED;
                    state = decodeState::CHECK_KEY_LENGTH;

                    ++cur;
                } else {
                    opcode = decodeOpcode::INDEXED_KEY;
                    state = decodeState::READ_INDEX;
                }

                indexed=c&0x40;
                neverIndexed=(c&0xf0)==0x10;
            }

            left=shift=0;

            break;

        }
        case decodeState::READ_TABLE_SIZE:{
            uint64_t nread;
            bool success;

            std::tie(left,nread,shift,success)=unpackInteger(src+cur,srclen-cur,left,shift,5);

            cur+=nread;

            if(!success){
                return false;
            }

            incstream.decoding_dynamic_table.setTableSize(static_cast<uint32_t>(left));

            state=decodeState::OPCODE;
            break;
        }
        case decodeState::READ_INDEX:{
            uint8_t prefixLen;

            if(opcode==decodeOpcode::INDEXED){
                prefixLen=7;

            }else if(indexed){
                prefixLen=6;
            }else {
                prefixLen=4;
            }


            uint64_t nread;
            bool success;

            std::tie(left,nread,shift,success)=unpackInteger(src+cur,srclen-cur,left,shift,prefixLen);

            cur += nread;

            if(!success||left==0||left>getMaxTableIndex(incstream.decoding_dynamic_table)){
                return false;
            }

            if(opcode==decodeOpcode::INDEXED){
                //解码后的请求数据添加添加至headers
                incstream.resquest_info.incoming_headers.emplace_back(*unpackIndexed(left,incstream.decoding_dynamic_table));


                state=decodeState::OPCODE;
            }else{
                keyIndex=left;
                state=decodeState::CHECK_VALUE_LENGTH;
            }

            break;

        }
        case decodeState::CHECK_KEY_LENGTH:{
            isHuffEncode=isHuffEncoded(src[cur]);
            state=decodeState::READ_KEY_LENGTH;
            left=shift=0;
            break;
        }
        case decodeState::READ_KEY_LENGTH:{
            uint64_t nread;
            bool success;

            std::tie(left,nread,shift,success)=unpackInteger(src+cur,srclen-cur,left,shift,7);

            cur+=nread;

            if(!success){

                return false;
            }

            state=isHuffEncode?decodeState::READ_KEY_HUFFMAN:decodeState::READ_KEY;

            break;
        }
        case decodeState::READ_KEY_HUFFMAN:{
            uint64_t nread;
            bool success;

            std::tie(nread,success)=unpackHuffman(_buf,src+cur,srclen-cur,left);

            cur+=nread;
            left-=nread;

            if(!success||left>0){
                return false;
            }

            keylen=_buf.size();

            state=decodeState::CHECK_VALUE_LENGTH;

            break;
        }
        case decodeState::READ_KEY:{
            const uint64_t nread=unpackString(_buf,src+cur,srclen-cur,left);

            cur+=nread;
            left-=nread;

            if(left>0){
                return false;
            }

            keylen=_buf.size();

            state=decodeState::CHECK_VALUE_LENGTH;

            break;
        }
        case decodeState::CHECK_VALUE_LENGTH:{
            isHuffEncode=isHuffEncoded(src[cur]);
            state=decodeState::READ_VALUE_LENGTH;
            left=shift=0;
            break;
        }
        case decodeState::READ_VALUE_LENGTH:{
            uint64_t nread;
            bool success;

            std::tie(left,nread,shift,success)=unpackInteger(src+cur,srclen-cur,left,shift,7);

            cur+=nread;

            if(!success){
                return false;
            }

            if(left==0){
                //添加至headers
                opcode==decodeOpcode::NOT_INDEXED?incstream.resquest_info.incoming_headers.emplace_back(unpackPairHeader(incstream.decoding_dynamic_table))
                                                :incstream.resquest_info.incoming_headers.emplace_back(unpackHeaderValue(incstream.decoding_dynamic_table));

                state=decodeState::OPCODE;

                break;
            }

            state=isHuffEncode?decodeState::READ_VALUE_HUFFMAN:decodeState::READ_VALUE;

            break;
        }
        case decodeState::READ_VALUE_HUFFMAN:{
            uint64_t nread;
            bool success;

            std::tie(nread,success)=unpackHuffman(_buf,src+cur,srclen-cur,left);

            cur+=nread;
            left-=nread;

            if(!success||left>0){
                return false;
            }

            //解码后的请求数据添加至headers
            opcode==decodeOpcode::NOT_INDEXED?incstream.resquest_info.incoming_headers.emplace_back(unpackPairHeader(incstream.decoding_dynamic_table))
                                            :incstream.resquest_info.incoming_headers.emplace_back( unpackHeaderValue(incstream.decoding_dynamic_table));

            state=decodeState::OPCODE;

            break;
        }
        case decodeState::READ_VALUE:{
            const uint64_t nread=unpackString(_buf,src+cur,srclen-cur,left);

            cur+=nread;
            left-=nread;

            if(left>0){
                return false;
            }

            //解码后的请求数据添加添加至headers
            opcode==decodeOpcode::NOT_INDEXED?incstream.resquest_info.incoming_headers.emplace_back(unpackPairHeader(incstream.decoding_dynamic_table))
                                            :incstream.resquest_info.incoming_headers.emplace_back( unpackHeaderValue(incstream.decoding_dynamic_table));

            state=decodeState::OPCODE;

            break;
        }
        }
    }



    return true;
}

const std::pair<std::string, std::string>* Decoder::unpackIndexed(const std::size_t index, const Hpack::DynamicTable &dynamicTable)
{

    //根据索引在相应的表中找到name+value
    if(Hpack::getStaticTableSize()>=index){//header位于静态表中
        return &Hpack::static_table[index-1];
    }else if(dynamicTable.size()+Hpack::getStaticTableSize()>=index){//header位于动态表中
        return &dynamicTable[index-Hpack::getStaticTableSize()-1];
    }


    return nullptr;
}

std::tuple<uint64_t, bool> Decoder::unpackHuffman(std::vector<char> &dest, const uint8_t *src, const std::size_t srclen, const uint64_t left)
{
    std::tuple<uint64_t,bool> res{left,false};

    if(srclen<left){
        std::get<uint64_t>(res)=srclen;
    }

    std::get<bool>(res)=Hpack::huff_decode(dest,src,std::get<uint64_t>(res));

    return res;
}

uint64_t Decoder::unpackString(std::vector<char> &dest, const uint8_t *src, const std::size_t srclen, uint64_t left)
{
    if(srclen<left){
        left=srclen;
    }
    dest.insert(dest.end(),src,src+left);

    return left;
}

std::tuple<uint64_t, uint64_t, uint8_t, bool> Decoder::unpackInteger(const uint8_t *src, const std::size_t srclen, const uint64_t initial, const uint8_t initialShift, const uint8_t prefix)
{
    std::tuple<uint64_t, uint64_t, uint8_t, bool> res {
        initial, 0, initialShift, true
    };

    uint64_t &num=std::get<0>(res);
    uint64_t &nread = std::get<1>(res);
    uint8_t &shift = std::get<uint8_t>(res);

    const uint8_t k=uint8_t(1<<prefix)-1;

    if(num==0){
        const uint8_t c=src[0];

        ++nread;

        if(k!=(c&k)){
            num=c&k;
            return res;
        }

        num=k;
        if(nread==srclen){
            return res;
        }

    }

    for(;nread<srclen;++nread){
        const uint8_t c=src[nread];

        uint32_t add=c&0x7f;

        if(add>(std::numeric_limits<uint32_t>::max()>>shift)){
            std::get<bool>(res)=false;
            return res;
        }

        add<<=shift;

        if(std::numeric_limits<uint32_t>::max()-add<num){
            std::get<bool>(res)=false;
            return res;
        }

        num+=add;

        if((c&(1<<7))==0){
            break;
        }

        shift+=7;
    }

    if(nread!=srclen){
        ++nread;
    }

    return res;
}

std::pair<std::string, std::string> Decoder::unpackPairHeader(Hpack::DynamicTable &dynamicTable)
{
    std::pair<std::string,std::string> header{std::string(_buf.cbegin(),_buf.cbegin()+long(keylen)),std::string(_buf.cbegin()+long(keylen),_buf.cend())};

    _buf.clear();

    if(indexed){
        dynamicTable.addHeader(header);
    }

    return header;
}

std::pair<std::string, std::string> Decoder::unpackHeaderValue(Hpack::DynamicTable &dynamicTable)
{
    auto entry=unpackIndexed(keyIndex,dynamicTable);

    if(!entry){
        return std::pair<std::string, std::string>();
    }

    std::pair<std::string, std::string>header{entry->first,std::string(_buf.cbegin(),_buf.cend())};

    keyIndex=0;
    _buf.clear();

    if(indexed){//加入动态表
        dynamicTable.addHeader(header);
    }

    return header;
}



