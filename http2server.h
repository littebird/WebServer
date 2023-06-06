#ifndef HTTP2SERVER_H
#define HTTP2SERVER_H

#include<string.h>
#include<vector>
#include<unordered_map>
#include<iostream>
#include"connection.h"
#include"frame.h"
#include"http2_stream.h"
#include "http/v2/decoder.h"


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
struct request_data{
    std::unordered_multimap<std::string,std::string> incoming_headers;
    std::unordered_multimap<std::string,std::string> incoming_data;
    std::unordered_multimap<std::string,std::string> incoming_files;
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
    static bool getNextHttp2FrameMeta(const Connection &sock,const std::chrono::milliseconds &timeout,
                                      std::vector<char> &buf,Frame incframe,
                                      long &read_size);
    static error_code parseHttp2Date(Frame& incframe,Http2_Stream& incstream,const uint8_t* src,const uint8_t* end);
    static error_code parseHttp2Headers(Frame& incframe,Http2_Stream& incstream,const uint8_t* src,const uint8_t* end);
    static error_code parseHttp2Settings(Frame& incframe,Http2_Stream& incstream,const uint8_t* src,const uint8_t* end);
    static uint32_t ntoh24(const void *src24) noexcept;
    static Http2_Stream &getStreamData(std::unordered_map<uint32_t,Http2_Stream> &streams,
                                     const uint32_t streamId,ConnectionData &conn);
private:
    Http2_Stream stream;
};

#endif // HTTP2SERVER_H
