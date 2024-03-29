#ifndef HTTP2SERVER_H
#define HTTP2SERVER_H

#include<string.h>
#include<vector>
#include<boost/asio/ssl.hpp>
#include<boost/asio.hpp>
#include<boost/bind.hpp>
#include<boost/asio/error.hpp>
#include<algorithm>
#include<random>
#include<chrono>
#include<thread>
#include"http/v2/hpack.h"
#include"http/v2/frame.h"
#include"http/v2/http2_stream.h"
#include"http/v2/decoder.h"
#include"http/v2/encoder.h"
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

    void process_data(boost::asio::ssl::stream<boost::asio::ip::tcp::socket> &socket,std::vector<char>& buf,
                      Frame& incframe,long &read_size,uint32_t last_stream_id);

    void after_process(boost::asio::ssl::stream<boost::asio::ip::tcp::socket>& socket, Http2_Stream& incstream);//解析process后的数据

    static std::array<uint8_t,FRAME_HEADER_SIZE> send_empty_settings(boost::asio::ssl::stream<boost::asio::ip::tcp::socket>& socket,
                                                                     const frameHeader_flag flags);//发送空的settings帧
    static uint8_t *set_frame_header(uint8_t *addr,const uint32_t framesize,
                                     const frame_type frametype,const frameHeader_flag frameflag,
                                     const uint32_t streamid);//设置帧首部

    static bool handleClientPreface(boost::asio::ssl::stream<boost::asio::ip::tcp::socket>& socket)noexcept;//获取连接前言数据

    static bool handleNextHttp2FrameMeta(std::vector<char> &buf,Frame& incframe,long& read_size,
                                         std::size_t bytes_transferred);//获取下一个帧数据
    static bool handleNextHttp2FrameMeta(boost::asio::ssl::stream<boost::asio::ip::tcp::socket>& socket,std::vector<char> &buf,
                                         Frame& incframe,long& read_size)noexcept;//获取下一个帧数据

    static Error_code parseHttp2Date(Frame& incframe,Http2_Stream& incstream,const uint8_t* src,const uint8_t* end);
    static Error_code parseHttp2Headers(Frame& incframe,Http2_Stream& incstream,const uint8_t* src,const uint8_t* end);
    static Error_code parseHttp2Settings(Frame& incframe,Http2_Stream& incstream,const uint8_t* src,const uint8_t* end);
    static Error_code parseHttp2WindowUpdate(Frame& incframe,Http2_Stream& incstream,const uint8_t* src,const uint8_t* end);
    static Error_code parseHttp2rstStream(Frame& incframe,Http2_Stream& incstream,const uint8_t* src,const uint8_t* end);
    static Error_code parseHttp2GoAway(Frame& incframe,Http2_Stream& incstream,const uint8_t* src,const uint8_t* end);
    static uint32_t ntoh24(const void *src24) noexcept;
    static Http2_Stream &getStreamData(std::unordered_map<uint32_t,Http2_Stream> &streams,
                                     const uint32_t streamId,ConnectionData &conn);

    void sendHeaders(boost::asio::ssl::stream<boost::asio::ip::tcp::socket>& socket
                     ,std::vector<std::pair<std::string,std::string>> &headers,Http2_Stream& incStream);//发送headers帧
    long sendData(boost::asio::ssl::stream<boost::asio::ip::tcp::socket>& socket,
                  const void *src,std::size_t size,Http2_Stream& incStream);//发送data帧
    static uint8_t getPaddingSize(std::size_t data_size);//获取填充字节大小
    static void sendWindowUpdate(boost::asio::ssl::stream<boost::asio::ip::tcp::socket>& socket,const Http2_Stream& incstream,const uint32_t size);
    static void sendGoAway(boost::asio::ssl::stream<boost::asio::ip::tcp::socket>& socket,
                       ConnectionData &conn,const uint32_t lastStreamId,
                       const Error_code errorcode);//发送goaway帧
    static void sendRstStream(boost::asio::ssl::stream<boost::asio::ip::tcp::socket>& socket,
                              Http2_Stream& incstream,const Error_code error);//发送rststream帧

    std::shared_ptr<std::unordered_map<uint32_t,Http2_Stream>> streams;
    std::shared_ptr<ConnectionData> _conn;
};

#endif // HTTP2SERVER_H
