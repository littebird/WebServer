#ifndef HEADERFRAME_H
#define HEADERFRAME_H

#include<string>
#include<unordered_map>
#include"frame.h"

enum headers_category{//报头的类别，它指示帧的角色。
    HCAT_REQUEST = 0,//表示报头帧正在打开新的流
    HCAT_RESPONSE = 1,//报头帧是第一个响应报头
    HCAT_PUSH_RESPONSE = 2,//针对推送的第一个报头帧
    HCAT_HEADERS = 3//其他类别

};

class HeaderFrame: public Frame//headers帧类
{
public:
    HeaderFrame(std::size_t padlen,
                uint8_t E,uint32_t stream_dp,
                headers_category cat,uint8_t wight,uint32_t padding,
                const std::unordered_map<std::string,std::string> &header_block);
    void set_category(headers_category cat);//设置报头帧标志
private:
    std::size_t m_padlen;//帧内填充的长度
    uint8_t m_E;//标识流依赖性是唯一的单位标志
    uint32_t m_stream_dp;//流依赖
    headers_category m_cat;//报头帧的类别
    uint8_t m_weight;//权重
    uint32_t m_padding;//填充字节
    std::unordered_map<std::string,std::string> m_header_block;//该帧所携带的数据
};

#endif // HEADERFRAME_H
