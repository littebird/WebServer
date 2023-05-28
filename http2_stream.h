#ifndef HTTP2_STREAM_H
#define HTTP2_STREAM_H

#include <string>
//#include<memory.h>
#include <boost/shared_ptr.hpp>
//流的状态
enum http2_stream_state{
    STREAM_IDLE,//流的初始状态
    STREAM_OPEN,
    STREAM_CLOSED,
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
    int stream_attach_item();//把帧加入到流中
    int stream_detach_item();//把帧从流中去除
    bool set_send_stream_state(std::shared_ptr<std::string> item);//发送帧时流状态的改变
    bool set_recieve_stream_state(std::shared_ptr<std::string> item);//接收帧时流状态的改变
    void set_weight();//设置权重    `
private:
    uint32_t m_stream_id;//流id
    http2_stream_state m_state;//流的状态
    uint32_t m_weight;//流的权重
};

#endif // HTTP2_STREAM_H
