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
struct ConnectionSettings{
    uint32_t header_table_size;
    uint32_t enable_push;
    uint32_t max_concurrent_streams;
    uint32_t initial_window_size;
    uint32_t max_frame_size;
    uint32_t max_header_list_size;
};
struct ConnectionData {
        ConnectionSettings client_settings;
        ConnectionSettings server_settings;
};
struct request_data{
    std::vector<std::pair<std::string,std::string>> incoming_headers;
    std::unordered_multimap<std::string,std::string> incoming_data;
    std::unordered_multimap<std::string,std::string> incoming_files;
};
class Http2_Stream
{
public:
    Http2_Stream();
    Http2_Stream(uint32_t streamid,const ConnectionData &conn);
    void stream_init(uint32_t stream_id,http2_stream_state state);//流的初始化
    bool set_send_stream_state(std::shared_ptr<Frame> &item);//发送帧时流状态的改变
    bool set_recieve_stream_state(std::shared_ptr<Frame> &item);//接收帧时流状态的改变
    void set_weight(uint32_t weight);//设置权重
    uint32_t get_stream_id();//返回流id

    Hpack::DynamicTable decoding_dynamic_table;
    Hpack::DynamicTable encoding_dynamic_table;
    uint32_t m_stream_id;//流id
    request_data resquest_info;//请求信息
    ConnectionData m_connectiondata;
    http2_stream_state m_state;//流的状态
    uint32_t m_weight;//流的权重
    uint8_t m_frame_type;
    void *reserved;//保留位

};

#endif // HTTP2_STREAM_H
