#include "http2server.h"

Http2Server::Http2Server()
{

}
void Http2Server::process(Connection &sock)
{
    Requset req;
    req.timeout=std::chrono::milliseconds(15000);

}

void Http2Server::send_empty_settings(const Connection &sock,const std::chrono::milliseconds &timeout,
                                      const headerframe_flag flags)
{
    constexpr uint32_t frame_size=0;//该帧的长度
    constexpr uint32_t frame_header_size=9;//头部帧长度
    std::array<uint8_t,frame_header_size+frame_size> buf;
    uint8_t *addr=buf.data();
    constexpr uint32_t stream_id=0;

}
