#ifndef GOAWAYFRAME_H
#define GOAWAYFRAME_H

#include<string>
#include"frame.h"
class GoAwayFrame : public Frame
{
public:
    GoAwayFrame(uint32_t last_stream_id,
                uint32_t error_code,uint8_t *opaque_data,
                std::size_t opaque_data_len,uint8_t reserved);
private:
    uint32_t m_last_stream_id;//最后一个流的流ID
    uint32_t m_error_code;//错误码，参考error_code结构体
    uint8_t *m_opaque_data;//额外的调试数据
    std::size_t m_opaque_data_len;//调试数据的长度
    uint8_t m_reserved;//保留位
};

#endif // GOAWAYFRAME_H
