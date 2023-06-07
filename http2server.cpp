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

    Http2_Stream &primary=streams.find(0)->second;
    this->stream=primary;
    std::size_t streams_process_count = 0;
    uint32_t last_stream_id = 0;

    Frame incframe;
    long read_size = 0;

    do{
        if(getNextHttp2FrameMeta(sock,req.timeout,buf,incframe,read_size)==false)
        {
            break;
        }

        const uint8_t *addr=reinterpret_cast<const uint8_t *>(buf.data())+FRAME_HEADER_SIZE;
        const uint8_t *end=addr+incframe._frame_hd.length;

        if(incframe._frame_hd.stream_id>last_stream_id)
        {
            last_stream_id=incframe._frame_hd.stream_id;
        }

        Http2_Stream &stream=getStreamData(streams,incframe._frame_hd.stream_id,conn);

        if(stream.m_state==http2_stream_state::STREAM_CLOSE)
        {
            //rststream;
            continue;
        }

        if(incframe._frame_hd.type!=frame_type::HTTP2_CONTINUATION)
        {
            stream.m_frame_type=incframe._frame_hd.type;

        }

        error_code result=error_code::NO_ERROR;

        switch(stream.m_frame_type)
        {
            case frame_type::HTTP2_DATA:
            {
                result=parseHttp2Date(incframe,stream,addr,end);

                if(stream.reserved)
                {
                    //设置窗口大小更新
                }
                break;
            }
            case frame_type::HTTP2_HEADERS:
            {
                result=parseHttp2Headers(incframe,stream,addr,end);
                if(incframe._frame_hd.flags&&frameHeader_flag::END_HEADERS)
                {


                }
                break;
            }
            case frame_type::HTTP2_PRIORITY:
            {
                result=error_code::NO_ERROR;
                break;
            }
            case frame_type::HTTP2_SETTINGS:
            {
                result=parseHttp2Settings(incframe,stream,addr,end);

                if(error_code::NO_ERROR==result&&(incframe._frame_hd.flags&frameHeader_flag::ACK)==false)
                {
                    send_empty_settings(sock,req.timeout,frameHeader_flag::ACK);
                }
                break;
            }
            case frame_type::HTTP2_PUSH_PROMISE:
            {
                result=error_code::NO_ERROR;
                break;
            }
            defult:
                result=error_code::PROTOCOL_ERROR;
                break;
        }

        if(result!=error_code::NO_ERROR)
        {
            stream.m_state=http2_stream_state::STREAM_CLOSE;
            //rststream

        }
        else if((incframe._frame_hd.flags & frameHeader_flag::END_STREAM)&& incframe._frame_hd.stream_id!=0)
        {
            //处理队列
        }

    }while(http2_stream_state::STREAM_CLOSE!=primary.m_state);

    goAway(sock,req.timeout,conn,last_stream_id,error_code::NO_ERROR);


}

void Http2Server::send_empty_settings(const Connection &sock,const std::chrono::milliseconds &timeout,
                                      const frameHeader_flag flags)
{
    constexpr uint32_t frame_size=0;//该帧的长度
    constexpr uint32_t frame_header_size=9;//头部帧长度
    std::array<uint8_t,frame_header_size+frame_size> buf;
    uint8_t *addr=buf.data();
    constexpr uint32_t stream_id=0;
    addr=set_frame_header(addr,frame_size,frame_type::HTTP2_SETTINGS,flags,stream_id);
    //to do发送数据
}

uint8_t *Http2Server::set_frame_header(uint8_t *addr, const uint32_t framesize,
                                       const frame_type frametype, const frameHeader_flag frameflag,
                                       const uint32_t streamid)
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
    return (addr+FRAME_HEADER_SIZE);
}

bool Http2Server::getNextHttp2FrameMeta(const Connection &sock,const std::chrono::milliseconds &timeout,
                                        std::vector<char> &buf,Frame incframe,
                                        long &read_size)
{
    const long length=long(incframe._frame_hd.length+FRAME_HEADER_SIZE);
    if(read_size<=length)
    {
        if(read_size==length)
        {
            read_size=0;
        }
        //to do 读数据
        if(read_size<long(FRAME_HEADER_SIZE)){
            return false;
        }
    }
    else{
        std::copy(buf.data()+length,buf.data()+read_size,buf.data());

        read_size-=length;
    }
    const uint8_t *addr=reinterpret_cast<const uint8_t *>(buf.data());

    incframe._frame_hd.length=ntoh24(addr);
    incframe._frame_hd.type=static_cast<frame_type>(*(addr + 3) );
    incframe._frame_hd.flags=static_cast<frameHeader_flag>(*(addr + 4) );
    incframe._frame_hd.stream_id = ::ntohl(*reinterpret_cast<const uint32_t *>(addr + 5) );

    return true;
}

error_code Http2Server::parseHttp2Date(Frame &incframe, Http2_Stream &incstream, const uint8_t *src, const uint8_t *end)
{//解析data帧
    if(incframe._frame_hd.stream_id==0){//流id为0
        return error_code::PROTOCOL_ERROR;
    }

    if(incstream.m_state!=http2_stream_state::STREAM_OPEN){//流状态异常
        return error_code::STREAM_CLOSED;
    }

    uint8_t padding=0;
    if(incframe._frame_hd.flags&frameHeader_flag::PADDED){//帧首部带有padded flag
        padding=*src;

        if(padding>=incframe._frame_hd.length){
            return error_code::PROTOCOL_ERROR;
        }

        src+=sizeof (uint8_t);
    }

    error_code err=error_code::NO_ERROR;

//    if(incstream.reserved){

//    }

    if(incframe._frame_hd.flags&frameHeader_flag::END_STREAM){//帧首部带有endstream flag
        incstream.m_state=http2_stream_state::STREAM_LOCAL_HALF_CLOSED;

        //中断数据接收

        incstream.reserved=nullptr;
    }

    return err;
}

error_code Http2Server::parseHttp2Headers(Frame &incframe, Http2_Stream &incstream, const uint8_t *src, const uint8_t *end)
{//解析header帧
    incstream.m_state=(incframe._frame_hd.flags&frameHeader_flag::END_STREAM)?http2_stream_state::STREAM_LOCAL_HALF_CLOSED
                                                                            :http2_stream_state::STREAM_OPEN;
    uint8_t padding = 0;
    if(incframe._frame_hd.flags&frameHeader_flag::PADDED){
        padding=*src;

        if(padding>=incframe._frame_hd.length){
            return error_code::PROTOCOL_ERROR;
        }

        src+=sizeof (uint8_t);
    }

    if(incframe._frame_hd.flags&frameHeader_flag::PRIORIY){
        const uint32_t depend_stream_id=::ntohl(*reinterpret_cast<const uint32_t *>(src))&~(uint32_t(1)<<31);

        src+=sizeof (uint32_t);

        incstream.set_weight(*src);

        src+=sizeof (uint8_t);
    }

    Decoder decoder;
    if(decoder.decode(src,std::size_t(end-src)-padding,incstream.dynamicTable)==false){
        return error_code::COMPRESSION_ERROR;
    }

    return error_code::NO_ERROR;

}

error_code Http2Server::parseHttp2Settings(Frame &incframe, Http2_Stream &incstream, const uint8_t *src, const uint8_t *end)
{//解析setting帧
    if(incframe._frame_hd.stream_id!=0){//streamid应该为0
        return error_code::PROTOCOL_ERROR;
    }

    if(incframe._frame_hd.length%(sizeof(uint16_t)+sizeof(uint32_t))){//检查长度
        return error_code::FRAME_SIZE_ERROR;
    }

    if(incstream.m_state!=http2_stream_state::STREAM_OPEN){
        incstream.m_state=http2_stream_state::STREAM_OPEN;
    }

    ConnectionSettings &settings=incstream.m_connectiondata.client_settings;

    while(src!=end){
        const settings_id setting = static_cast<settings_id>(
                        ntohs(*reinterpret_cast<const uint16_t *>(src) )
                    );

        src+=sizeof(uint16_t);

        const uint32_t value=::htonl(*reinterpret_cast<const uint32_t*>(src));

        switch(setting){
        case settings_id::SETTINGS_HEADER_TABLE_SIZE :{
            settings.header_table_size=value;
            break;
        }
        case settings_id::SETTINGS_ENABLE_PUSH:{
            if(value>1){
                return error_code::PROTOCOL_ERROR;
            }

            settings.enable_push=value;

            break;
        }
        case settings_id::SETTINGS_MAX_CONCURRENT_STREAMS:{
            settings.max_concurrent_streams=value;
            break;


        }
        case settings_id::SETTINGS_INITIAL_WINDOW_SIZE:{
            if(value>=uint32_t(1)<<31){
                return error_code::FLOW_CONTROL_ERROR;
            }

            settings.initial_window_size=value;

            break;
        }
        case settings_id::SETTINGS_MAX_FRAME_SIZE:{
            if(value<(1<<14)||value>=(1<<24)){
                return error_code::PROTOCOL_ERROR;
            }

            settings.max_frame_size=value;

            break;
        }
        case settings_id::SETTINGS_MAX_HEADER_LIST_SIZE:{
            settings.max_header_list_size=value;

            break;
        }
        default:
            break;
        }

    }

    return error_code::NO_ERROR;
}

uint32_t Http2Server::ntoh24(const void *src24) noexcept
{
    static endianness endian=endianness::INIT;

            union {
                uint32_t ui;
                uint8_t c[sizeof(uint32_t)];
            } x;

            if (endian == endianness::INIT) {
                x.ui = 0x01;
                endian = (x.c[3] == 0x01) ? endianness::BIGE : endianness::LITE;
            }

            if (endian == endianness::BIGE) {
                return *reinterpret_cast<const uint32_t *>(src24) >> 8;
            }

            const uint8_t *addr = reinterpret_cast<const uint8_t *>(src24);

            x.ui = 0;

            x.c[0] = addr[2];
            x.c[1] = addr[1];
            x.c[2] = addr[0];

            return x.ui;
}

Http2_Stream &Http2Server::getStreamData(std::unordered_map<uint32_t,Http2_Stream> &streams,
                                        const uint32_t streamId,ConnectionData &conn)
{
    auto it=streams.find(streamId);

    if(streams.end()!=it)
    {
        return it->second;
    }

    return streams.emplace(streamId,Http2_Stream(streamId,conn)).first->second;
}

void Http2Server::goAway(Connection &sock, const std::chrono::milliseconds &timeout,
                         ConnectionData &conn, const uint32_t lastStreamId,
                         const error_code errorcode)
{
    constexpr uint32_t frame_size = sizeof(uint32_t) * 2;

    std::array<uint8_t,FRAME_HEADER_SIZE + frame_size> buf;

    uint8_t *addr=buf.data();

    addr=set_frame_header(addr,frame_size,frame_type::HTTP2_RST_STREAM,frameHeader_flag::EMPTY,0);

    *reinterpret_cast<uint32_t *>(addr) = ::htonl(lastStreamId);

    *reinterpret_cast<uint32_t *>(addr + sizeof(uint32_t) ) = ::htonl(static_cast<const uint32_t>(errorcode));

    //发送数据

}
