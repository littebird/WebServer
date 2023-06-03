#ifndef HTTP2SERVER_H
#define HTTP2SERVER_H

#include <string>
#include <arpa/inet.h>
#include "frame.h"
#include "http2_stream.h"
#include "http/v2/decoder.h"

class Http2Server
{
public:
    Http2Server();

    static error_code parseHttp2Date(Frame& incframe,Http2_Stream& incstream,const uint8_t* src,const uint8_t* end);
    static error_code parseHttp2Headers(Frame& incframe,Http2_Stream& incstream,const uint8_t* src,const uint8_t* end);
    static error_code parseHttp2Settings(Frame& incframe,Http2_Stream& incstream,const uint8_t* src,const uint8_t* end);
};

#endif // HTTP2SERVER_H
