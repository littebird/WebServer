#include "encoder.h"

Encoder::Encoder()
{

}

void Encoder::encode(std::vector<char> &dest, const std::vector<std::pair<std::string, std::string> > &headers, Hpack::DynamicTable &dynamicTable)
{
    for(auto const &header:headers){
        std::size_t index;
        bool is_full_match;

        std::tie(index,is_full_match)=findHeaderInTable(header,dynamicTable);


        if(is_full_match){
            packIndex(dest,index);
            continue;
        }

        const bool indexing=true;//目前默认带索引

        if(indexing){
            dynamicTable.addHeader(header);
        }

        if(index==0){
            packPairHeader(dest,header,indexing);
        }else{
            packHeaderValue(dest,index,header,indexing);
        }
    }
}

void Encoder::packInteger(std::vector<char> &dest, uint64_t num, const uint8_t prefix)
{//整数编码
    const uint64_t k = (1 << prefix) - 1;


    if(num<k){
        dest.emplace_back(num);
        return;
    }

    dest.emplace_back(k);
    num-=k;
    for(;;){
        if(num<128){
            dest.emplace_back(num);
            break;
        }

        dest.emplace_back(0x80|(num&0x7f));

        num>>=7;
        if(num==0)
            break;
    }

}

void Encoder::packIndex(std::vector<char> &dest, const std::size_t index)
{
    const std::size_t head=dest.size();

    packInteger(dest,index,7);

    dest[head]|=0x80;

}

std::tuple<std::size_t, bool> Encoder::findHeaderInTable(const std::pair<std::string, std::string> &header, const Hpack::DynamicTable &dynamicTable)
{//返回索引,是否匹配
    std::tuple<std::size_t, bool> res {0,false};

    std::size_t& index=std::get<std::size_t>(res);//获取索引
    bool& is_full_match=std::get<bool>(res);//是否匹配

    for(std::size_t i=0;i<Hpack::getStaticTableSize();++i){
        auto const&pair=Hpack::static_table[i];

        if(pair.first==header.first){
            index=i+1;
            if(pair.second==header.second){
                is_full_match=true;
                return res;
            }
        }else if (index != 0) {
            break;
        }
    }

    for(std::size_t i=0;i<dynamicTable.size();++i){
        auto const& pair=dynamicTable[i];

        if(pair.first==header.first){
            index=i+Hpack::getStaticTableSize()+1;
            if(pair.second==header.second){
                is_full_match=true;
                break;
            }
        }
    }

    return res;
}

void Encoder::packString(std::vector<char> &dest, const std::string &str)
{
    const u_int8_t* ustr=reinterpret_cast<const uint8_t* >(str.data());//转换为uint8_t*类型
    const std::size_t huff_len=Hpack::huff_encode_len(ustr,str.length());//编码长度

    if(huff_len<str.length()){
        const  std::size_t head=dest.size();

        packInteger(dest,huff_len,7);
        Hpack::huff_encode(dest,ustr,str.length());

        dest[head]|=0x80;
    }else{
        packInteger(dest,str.length(),7);

        dest.insert(dest.end(),str.cbegin(),str.cend());
    }
}

void Encoder::packPairHeader(std::vector<char> &dest, const std::pair<std::string, std::string> &header, const bool indexing)
{
    dest.emplace_back(indexing?0x40:0x00);
    packString(dest,header.first);
    packString(dest,header.second);
}

void Encoder::packHeaderValue(std::vector<char> &dest, const std::size_t keyIndex, const std::pair<std::string, std::string> &header, const bool indexing)
{
    const std::size_t head=dest.size();

    const uint8_t prefix=indexing?6:4;

    packInteger(dest,keyIndex,prefix);

    dest[head]|=indexing?0x40:0x00;

    packString(dest,header.second);

}
