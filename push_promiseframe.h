#ifndef PUSH_PROMISEFRAME_H
#define PUSH_PROMISEFRAME_H

#include<string>
#include"frame.h"

//推送帧
class Push_PromiseFrame : public Frame
{
public:
    Push_PromiseFrame(const frame_hd &frame_hd,std::size_t padlen,
                      nv *nva,std::size_t nvlen,
                      uint32_t promise_stream_id,uint8_t reserved);
private:
    frame_hd m_frame_hd;//帧首部
    std::size_t m_padlen;//填充长度
    nv *m_nva;//指向键值对的指针
    std::size_t m_nvlen;//键值对长度
    uint32_t m_promise_stream_id;//保留的流id
    uint8_t m_reserved;//保留位
};

#endif // PUSH_PROMISEFRAME_H
