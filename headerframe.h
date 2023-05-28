#ifndef HEADERFRAME_H
#define HEADERFRAME_H

#include<string>
enum headerframe_flag{//头部帧标志
    END_STREAM,
    END_HEADERS,
    PADDED,
    PRIORIY,
};
enum headers_category{//报头的类别，它指示帧的角色。
    HCAT_REQUEST = 0,//表示报头帧正在打开新的流
    HCAT_RESPONSE = 1,//报头帧是第一个响应报头
    HCAT_PUSH_RESPONSE = 2,//针对推送的第一个报头帧
    HCAT_HEADERS = 3//其他类别

};

class HeaderFrame//headers帧类
{
public:
    HeaderFrame(const std::string &frame_hd,std::size_t padlen,
                headerframe_flag flag,uint8_t E,
                uint32_t stream_dp,headers_category cat,
                uint8_t wight,uint32_t padding,
                uint32_t stream_id);
    void set_frame_flag(headerframe_flag flag);//设置头部帧标志
    void set_category(headers_category cat);//设置报头帧标志
    headerframe_flag get_flag();//返回当前头部帧的标记

private:
    std::string m_frame_hd;//帧首部
    std::size_t m_padlen;//帧内填充的长度
    headerframe_flag m_flag;//帧的标记
    uint8_t m_E;//标识流依赖性是唯一的单位标志
    uint32_t m_stream_dp;//流依赖
    headers_category m_cat;//报头帧的类别
    uint8_t m_weight;//权重
    uint32_t m_padding;//填充字节
    uint32_t m_stream_id;//流id
};

#endif // HEADERFRAME_H
