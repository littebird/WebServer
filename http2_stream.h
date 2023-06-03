#ifndef HTTP2_STREAM_H
#define HTTP2_STREAM_H

#include <string>
#include <boost/shared_ptr.hpp>
#include"frame.h"
#include"http/v2/hpack.h"
//流的状态
enum http2_stream_state{
    STREAM_IDLE,//流的初始状态
    STREAM_OPEN,
    STREAM_CLOSE,
    STREAM_LOCAL_RESERVED,
    STREAM_REMOTE_RESERVED,
    STREAM_LOCAL_HALF_CLOSED,
    STREAM_REMOTE_HALF_CLOSED,
};

class Http2_Stream
{
public:
    Http2_Stream();
    void stream_init(uint32_t stream_id,http2_stream_state state);//流的初始化
    bool set_send_stream_state(std::shared_ptr<Frame> &item);//发送帧时流状态的改变
    bool set_recieve_stream_state(std::shared_ptr<Frame> &item);//接收帧时流状态的改变
    void set_weight(uint32_t weight);//设置权重
    uint32_t get_stream_id();//返回流id

    Hpack::DynamicTable dynamicTable;
    uint32_t m_stream_id;//流id
    http2_stream_state m_state;//流的状态
    uint32_t m_weight;//流的权重
    void *reserved;//保留位

};

#endif // HTTP2_STREAM_H
