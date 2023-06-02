#ifndef HTTP2SERVER_H
#define HTTP2SERVER_H

#include<string.h>
#include"connection.h"
#include"frame.h"
struct Requset{
    std::unordered_map<std::string,std::string> outgoing_headers;
    std::string host;
    std::string path;
    std::string method;
    std::chrono::milliseconds timeout;//超时时间

};

class Http2Server
{
public:
    Http2Server();
    void process(Connection &sock);//http2处理的所有过程
    static void send_empty_settings(const Connection &sock,const std::chrono::milliseconds &timeout,
                                    const headerframe_flag flags);//发送空的settings帧

};

#endif // HTTP2SERVER_H
