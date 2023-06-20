#ifndef DECODER_H
#define DECODER_H

#include "hpack.h"
#include "hpack.cpp"
#include <tuple>
#include <numeric>
#include "../../http2_stream.h"

enum decodeOpcode{//解码是否存在索引
    NONE,
    INDEXED,//有索引
    NOT_INDEXED,//没有索引
    INDEXED_KEY,//仅有key的索引
};

enum decodeState{//解码状态
    OPCODE,
    READ_TABLE_SIZE,
    READ_INDEX,
    CHECK_KEY_LENGTH,
    READ_KEY_LENGTH,
    READ_KEY_HUFFMAN,
    READ_KEY,
    CHECK_VALUE_LENGTH,
    READ_VALUE_LENGTH,
    READ_VALUE_HUFFMAN,
    READ_VALUE,
};

class Decoder
{
public:
    Decoder();

    bool decode(const uint8_t *src,const std::size_t srclen,Http2_Stream& incstream);//headers帧头部块解压缩数据

    void init(){
        _buf.clear();

        keyIndex=0;
        keylen=0;

        opcode=decodeOpcode::NONE;
        state=decodeState::OPCODE;

        left=0;
        shift=0;

        indexed=false;
        neverIndexed=false;
        isHuffEncode=false;
    }

    static std::size_t getMaxTableIndex(const Hpack::DynamicTable& dynamicTable){//返回最大索引
        return Hpack::getStaticTableSize()+dynamicTable.size();
    }
    static const std::pair<std::string,std::string> *unpackIndexed(const std::size_t index,const Hpack::DynamicTable& dynamicTable);//通过index获取name-value
    static bool isHuffEncoded(const uint8_t c){//是否使用huffman编码
        return c&(1<<7);
    }
    static std::tuple<uint64_t,bool> unpackHuffman(std::vector<char>& dest,const uint8_t* src,const std::size_t srclen,const uint64_t left);

    static uint64_t unpackString(std::vector<char>& dest,const uint8_t* src,const std::size_t srclen,uint64_t left);//解码字符串字面

    static std::tuple<uint64_t, uint64_t, uint8_t, bool> unpackInteger(const uint8_t* src,const std::size_t srclen,const uint64_t initial,const uint8_t initialShift,const uint8_t prefix );//解码整数表示

    std::pair<std::string, std::string> unpackPairHeader(Hpack::DynamicTable& dynamicTable);//解码name+value
    std::pair<std::string, std::string> unpackHeaderValue(Hpack::DynamicTable& dynamicTable);//解码value

private:
    std::vector<char> _buf;
    std::size_t keyIndex;
    std::size_t keylen;
    decodeOpcode opcode;
    decodeState state;
    uint64_t left;
    uint8_t shift;
    bool indexed;
    bool neverIndexed;
    bool isHuffEncode;

};

#endif // DECODER_H
