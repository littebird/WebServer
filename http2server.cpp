#include "http2server.h"

Http2Server::Http2Server()
{

}
void Http2Server::process(Connection &sock)
{
    Requset req;
    req.timeout=std::chrono::milliseconds(15000);
    ConnectionData conn;
    send_empty_settings(sock,req.timeout,frameHeader_flag::EMPTY);

    //to do 如果有错发送goaway帧

    conn.client_settings=ConnectionSettings{4096,1,0,(1<<16)-1,1<<14,0};
    conn.server_settings=ConnectionSettings{4096,1,0,(1<<16)-1,1<<14,0};

    std::vector<char> buf(conn.server_settings.max_frame_size);
    std::unordered_map<uint32_t,Http2_Stream> streams{
      {0,Http2_Stream(0,conn)}
    };

    this->stream=streams.find(0)->second;
    std::size_t streams_process_count = 0;
    uint32_t last_stream_id = 0;

}

void Http2Server::send_empty_settings(const Connection &sock,const std::chrono::milliseconds &timeout,
                                      const frameHeader_flag flags)
{
    constexpr uint32_t frame_size=0;//该帧的长度
    constexpr uint32_t frame_header_size=9;//头部帧长度
    std::array<uint8_t,frame_header_size+frame_size> buf;
    uint8_t *addr=buf.data();
    constexpr uint32_t stream_id=0;
    addr=set_frame_header(addr,frame_size,frame_type::NGHTTP2_SETTINGS,flags,stream_id);
    //to do发送空的settings帧
}

uint8_t *Http2Server::set_frame_header(uint8_t *addr, const uint32_t framesize, const frame_type frametype, const frameHeader_flag frameflag, const uint32_t streamid)
{
    static endianness endian=endianness::INIT;

    union{
        uint32_t ui;
        uint8_t c[sizeof(uint32_t)];
    }x;

    if(endian==endianness::INIT)
    {
        x.ui=0x01;
        endian=(x.c[3]==0x01) ? endianness::BIGE : endianness::LITE;
    }

    x.ui=framesize;

    if(endian==endianness::BIGE)
    {
        x.ui<<=8;
    }
    else{
        uint8_t c=x.c[0];
        x.c[0]=x.c[2];
        x.c[2]=c;
    }

    std::copy(x.c,x.c+3,reinterpret_cast<uint8_t *>(addr));//以上是将主机字节顺序转换为网络字节顺序
    *(addr + 3) = static_cast<const uint8_t>(frametype);
    *(addr + 4) = static_cast<const uint8_t>(frameflag);
    *reinterpret_cast<uint32_t *>(addr+5)=::htonl(streamid);
    return (addr+9);
}

