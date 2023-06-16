#ifndef HTTP2SERVER_H
#define HTTP2SERVER_H

#include<string.h>
#include<vector>
#include<boost/asio/ssl.hpp>
#include<boost/asio.hpp>
#include<chrono>
#include"frame.h"
#include"settingsframe.h"
#include"http2_stream.h"
#include"http/v2/decoder.h"
#include"http/httprequest.h"
#include"http/httpresponse.h"


enum endianness{
    INIT=0,
    LITE=1,
    BIGE=2
};


class Http2Server
{
public:
    Http2Server();

    void process(std::unique_ptr<boost::asio::ssl::stream<boost::asio::ip::tcp::socket>>& socket);//http2处理的过程

    void after_process(const Http2_Stream& incstream);//解析process后的数据

    static std::array<uint8_t,FRAME_HEADER_SIZE> send_empty_settings(boost::asio::ssl::stream<boost::asio::ip::tcp::socket>& socket,
                                                                     const frameHeader_flag flags);//发送空的settings帧
    static uint8_t *set_frame_header(uint8_t *addr,const uint32_t framesize,
                                     const frame_type frametype,const frameHeader_flag frameflag,
                                     const uint32_t streamid);//设置帧首部

    static bool getClientPreface(boost::asio::ssl::stream<boost::asio::ip::tcp::socket>& socket)noexcept;//获取连接前言数据

    static bool getNextHttp2FrameMeta(boost::asio::ssl::stream<boost::asio::ip::tcp::socket> &socket,//获取下一个帧数据
                                      std::vector<char> &buf,Frame& incframe,
                                      long &read_size)noexcept;
    static Error_code parseHttp2Date(Frame& incframe,Http2_Stream& incstream,const uint8_t* src,const uint8_t* end);
    static Error_code parseHttp2Headers(Frame& incframe,Http2_Stream& incstream,const uint8_t* src,const uint8_t* end);
    static Error_code parseHttp2Settings(Frame& incframe,Http2_Stream& incstream,const uint8_t* src,const uint8_t* end);
    static uint32_t ntoh24(const void *src24) noexcept;
    static Http2_Stream &getStreamData(std::unordered_map<uint32_t,Http2_Stream> &streams,
                                     const uint32_t streamId,ConnectionData &conn);
    static void goAway(ConnectionData &conn,const uint32_t lastStreamId,
                       const Error_code errorcode);
private:
    std::shared_ptr<Http2_Stream> stream_;
};

#endif // HTTP2SERVER_H
