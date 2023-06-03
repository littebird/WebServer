#ifndef HTTP2SERVER_H
#define HTTP2SERVER_H

#include<string.h>
#include<vector>
#include"connection.h"
#include"frame.h"
#include"http2_stream.h"
struct Requset{
    std::unordered_map<std::string,std::string> outgoing_headers;
    std::string host;
    std::string path;
    std::string method;
    std::chrono::milliseconds timeout;//超时时间

};
enum endianness{
    INIT=0,
    LITE=1,
    BIGE=2
};
class Http2Server
{
public:
    Http2Server();
    void process(Connection &sock);//http2处理的所有过程
    static void send_empty_settings(const Connection &sock,const std::chrono::milliseconds &timeout,
                                    const frameHeader_flag flags);//发送空的settings帧
    static uint8_t *set_frame_header(uint8_t *addr,const uint32_t framesize,
                                     const frame_type frametype,const frameHeader_flag frameflag,
                                     const uint32_t streamid);
private:
    Http2_Stream stream;
};

#endif // HTTP2SERVER_H
