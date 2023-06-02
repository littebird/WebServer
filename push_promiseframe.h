#ifndef PUSH_PROMISEFRAME_H
#define PUSH_PROMISEFRAME_H

#include<string>
#include"frame.h"

//推送帧
class Push_PromiseFrame : public Frame
{
public:
    Push_PromiseFrame(std::size_t padlen,
                      uint32_t promise_stream_id,uint8_t reserved);
private:
    std::size_t m_padlen;//填充长度
    uint32_t m_promise_stream_id;//保留的流id
    uint8_t m_reserved;//保留位
};

#endif // PUSH_PROMISEFRAME_H
