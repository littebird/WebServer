#ifndef ENCODER_H
#define ENCODER_H

#include "hpack.h"
#include "hpack.cpp"
#include <tuple>

class Encoder
{
public:
    Encoder();
    static void encode(std::vector<char>& dest,const std::vector<std::pair<std::string,std::string>>& headers,Hpack::DynamicTable& dynamicTable);
    static void packInteger(std::vector<char>& dest,uint64_t num,const uint8_t prefix);//处理整数表示
    static void packIndex(std::vector<char> &dest,const std::size_t index); //编码索引
    //从动态表/静态表中寻找该header
    static std::tuple<std::size_t, bool> findHeaderInTable(const std::pair<std::string, std::string> &header,const Hpack::DynamicTable &dynamicTable);
    static void packString(std::vector<char>& dest,const std::string& str);//处理字符串字面表示
    static void packPairHeader(std::vector<char>& dest,const std::pair<std::string,std::string>& header,const bool indexing);//处理索引为0时name+value编码
    //处理索引存在+value编码
    static void packHeaderValue(std::vector<char>& dest,const std::size_t keyIndex,const std::pair<std::string,std::string>& header,const bool indexing);


};

#endif // ENCODER_H
