#ifndef HTTP2SERVER_H
#define HTTP2SERVER_H

#include<string.h>
#include<vector>
#include<unordered_map>
#include<boost/asio/ssl.hpp>
#include<boost/asio.hpp>
#include<algorithm>
#include<random>
#include"http/v2/hpack.h"
#include<chrono>
#include"frame.h"
#include"settingsframe.h"
#include"http2_stream.h"
#include"http/v2/decoder.h"
#include"http/v2/encoder.h"

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

    void process(std::unique_ptr<boost::asio::ssl::stream<boost::asio::ip::tcp::socket>>& socket);//http2处理的所有过程
    static std::array<uint8_t,FRAME_HEADER_SIZE> send_empty_settings(
                                    const frameHeader_flag flags);//发送空的settings帧
    static uint8_t *set_frame_header(uint8_t *addr,const uint32_t framesize,
                                     const frame_type frametype,const frameHeader_flag frameflag,
                                     const uint32_t streamid);

    static bool getClientPreface(boost::asio::ssl::stream<boost::asio::ip::tcp::socket>& socket);
    static bool getNextHttp2FrameMeta(boost::asio::ssl::stream<boost::asio::ip::tcp::socket> &socket,
                                      std::vector<char> &buf,Frame& incframe,
                                      long &read_size);
    static Error_code parseHttp2Date(Frame& incframe,Http2_Stream& incstream,const uint8_t* src,const uint8_t* end);
    static Error_code parseHttp2Headers(Frame& incframe,Http2_Stream& incstream,const uint8_t* src,const uint8_t* end);
    static Error_code parseHttp2Settings(Frame& incframe,Http2_Stream& incstream,const uint8_t* src,const uint8_t* end);
    static uint32_t ntoh24(const void *src24) noexcept;
    static Http2_Stream &getStreamData(std::unordered_map<uint32_t,Http2_Stream> &streams,
                                     const uint32_t streamId,ConnectionData &conn);
    void sendHeaders(std::vector<std::pair<std::string,std::string>> &headers,const uint32_t streamId);
    long sendData(const void *src,std::size_t size,const uint32_t streamId);
    static uint8_t getPaddingSize(std::size_t data_size);
    static void goAway(const std::chrono::milliseconds &timeout,
                       ConnectionData &conn,const uint32_t lastStreamId,
                       const Error_code errorcode);
    std::shared_ptr<Http2_Stream> stream;
};

#endif // HTTP2SERVER_H
