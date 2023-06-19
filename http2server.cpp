#include "http2server.h"
#include <arpa/inet.h>

Http2Server::Http2Server(std::shared_ptr<std::mutex> &Mutex):conn(new ConnectionData),_Mutex(Mutex)
{

    streams=std::make_shared<std::unordered_map<uint32_t,Http2_Stream>>();
    auto stream=Http2_Stream(0,*conn);
    streams->emplace(0,stream);
}

void Http2Server::process(std::unique_ptr<boost::asio::ssl::stream<boost::asio::ip::tcp::socket> > &socket)
{

    if(!getClientPreface(*socket)){
        //to do 如果连接前言出错发送goaway帧
        constexpr uint32_t last_stream_id = 0;
        goAway(*socket,*conn,last_stream_id,Error_code::PROTOCOL_ERROR);
        return;
    }
    conn->client_settings=ConnectionSettings{4096,1,0,(1<<16)-1,1<<14,256};
    conn->server_settings=ConnectionSettings{4096,1,0,(1<<16)-1,1<<14,256};


    std::vector<char> buf(MAX_FRAME_SIZE);



    Http2_Stream &primary=streams->find(0)->second;

    std::size_t streams_process_count = 0;
    uint32_t last_stream_id = 0;


    Frame incframe;
    long read_size = 0;
    do{
        if(getNextHttp2FrameMeta(*socket,buf,incframe,read_size)==false)
        {
            std::cout<<"false get frame "<<std::endl;
            break;
        }

        std::cout<<uint16_t(incframe._frame_hd.type)<<std::endl;

        const uint8_t *addr=reinterpret_cast<const uint8_t *>(buf.data())+FRAME_HEADER_SIZE;
        const uint8_t *end=addr+incframe._frame_hd.length;



        if(incframe._frame_hd.stream_id>last_stream_id)
        {
            last_stream_id=incframe._frame_hd.stream_id;
        }


        Http2_Stream &stream=getStreamData(*streams,incframe._frame_hd.stream_id,*conn);

        std::cout<<"stream id: "<<stream.get_stream_id()<<std::endl;
//        std::cout<<"streams size: "<<streams->size()<<std::endl;

        if(stream.m_state==http2_stream_state::STREAM_CLOSE)
        {
            //rststream;
            std::cout<<"stream_close: rststream"<<std::endl;
            sendRstStream(*socket,stream,Error_code::STREAM_CLOSED);
            continue;

        }
        if(incframe._frame_hd.type!=frame_type::HTTP2_CONTINUATION)
        {
            stream.m_frame_type=incframe._frame_hd.type;

        }

//        std::cout<<(uint16_t)incframe._frame_hd.type<<std::endl;

        Error_code result=Error_code::NO_ERROR;

        switch(stream.m_frame_type)
        {
            case frame_type::HTTP2_DATA:
            {
                std::cout<<"dataframe"<<std::endl;

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
                    std::cout<<"end_headers"<<std::endl;

                    after_process(*socket,stream);


//                    for(auto const& table:decoding_dynamic_table.getEntrys()){
//                        std::cout<<table.first<<std::endl;
//                    }

                }
                break;
            }
            case frame_type::HTTP2_PRIORITY:
            {
                result=Error_code::NO_ERROR;
                break;
            }
            case frame_type::HTTP2_RST_STREAM:
            {
                result=parseHttp2rstStream(incframe,stream,addr,end);
                break;
            }
            case frame_type::HTTP2_SETTINGS:
            {
                result=parseHttp2Settings(incframe,stream,addr,end);

                if(Error_code::NO_ERROR==result&&(incframe._frame_hd.flags&frameHeader_flag::ACK)==false)
                {
                    send_empty_settings(*socket,frameHeader_flag::ACK);

                }
                break;
            }
            case frame_type::HTTP2_PUSH_PROMISE:
            {
                result=Error_code::NO_ERROR;
                break;
            }
            case frame_type::HTTP2_GOAWAY:
            {
                std::cout<<"go away"<<std::endl;
                break;
            }
            case frame_type::HTTP2_WINDOW_UPDATE:
            {

                std::cout<<"window update"<<std::endl;
                result=parseHttp2WindowUpdate(incframe,stream,addr,end);
                break;
            }
            defult:
                result=Error_code::PROTOCOL_ERROR;
                break;
        }

        if(result!=Error_code::NO_ERROR)
        {
            stream.m_state=http2_stream_state::STREAM_CLOSE;
            //rststream
            sendRstStream(*socket,stream,Error_code::STREAM_CLOSED);

        }
        else if((incframe._frame_hd.flags & frameHeader_flag::END_STREAM)&& incframe._frame_hd.stream_id!=0)
        {
            //处理队列
//            std::cout<<"stream id: "<<stream.get_stream_id()<<std::endl;

        }

    }while(http2_stream_state::STREAM_CLOSE!=primary.m_state);


    goAway(*socket,*conn,last_stream_id,Error_code::NO_ERROR);

}

void Http2Server::after_process(boost::asio::ssl::stream<boost::asio::ip::tcp::socket> &socket, Http2_Stream& incstream)
{//解析请求的信息
    auto req=std::make_shared<HttpRequest>();

    for(auto const& header:incstream.resquest_info.incoming_headers){
        req->header_kv.emplace(header.first,header.second);
        if(header.first==":method")
            req->m_method=header.second;
        if(header.first==":path"){
            req->m_uri=header.second;
            req->parse_uri();
        }
    }

    auto res=std::make_shared<HttpResponse>("../WebServer/resource",req->path(),true);
    std::vector<std::pair<std::string,std::string>> outgoing_headers;

    res->buildH2Response(outgoing_headers);


    sendHeaders(socket,outgoing_headers,incstream);//发送headers帧

    sendData(socket,res->m_response_body.data(),res->m_response_body.size(),incstream);//发送data帧


}

std::array<uint8_t,FRAME_HEADER_SIZE> Http2Server::send_empty_settings(boost::asio::ssl::stream<boost::asio::ip::tcp::socket>& socket,
                                                                       const frameHeader_flag flags)
{
    constexpr uint32_t frame_size=0;//该帧的长度
    //constexpr uint32_t frame_header_size=9;//头部帧长度
    std::array<uint8_t,FRAME_HEADER_SIZE+frame_size> buf;
    uint8_t *addr=buf.data();
    constexpr uint32_t stream_id=0;
    addr=set_frame_header(addr,frame_size,frame_type::HTTP2_SETTINGS,flags,stream_id);
    //to do发送数据

    socket.write_some(boost::asio::buffer(buf,buf.size()));

    return buf;
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

    std::copy(x.c,x.c+3,addr);//以上是将主机字节顺序转换为网络字节顺序
    *(addr + 3) = static_cast<const uint8_t>(frametype);
    *(addr + 4) = static_cast<const uint8_t>(frameflag);
    *reinterpret_cast<uint32_t *>(addr+5)=::htonl(streamid);

    return (addr+FRAME_HEADER_SIZE);
}

bool Http2Server::getClientPreface (boost::asio::ssl::stream<boost::asio::ip::tcp::socket> &socket)noexcept
{
    std::array<uint8_t, 24> buf;


    const long read_size=socket.read_some(boost::asio::buffer(buf));

    if (buf.size() != read_size) {
        return false;
    }

    static constexpr char client_preface_data[] = "PRI * HTTP/2.0\r\n\r\nSM\r\n\r\n";

    const uint64_t *left = reinterpret_cast<const uint64_t *>(client_preface_data);
    const uint64_t *right = reinterpret_cast<const uint64_t *>(buf.data() );

    uint64_t compare = 0;

    compare |= left[0] ^ right[0];
    compare |= left[1] ^ right[1];
    compare |= left[2] ^ right[2];

    return compare==0;
}

bool Http2Server::getNextHttp2FrameMeta(boost::asio::ssl::stream<boost::asio::ip::tcp::socket> &socket,
                                        std::vector<char> &buf,Frame& incframe,
                                        long &read_size)noexcept
{
    const long length=long(incframe._frame_hd.length+FRAME_HEADER_SIZE);
    if(read_size<=length)
    {
        if(read_size==length)
        {
            read_size=0;
        }


        //to do 读数据
        read_size=socket.read_some(boost::asio::buffer(buf.data()+read_size,buf.size()-std::size_t(read_size)));


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

Error_code Http2Server::parseHttp2Date(Frame &incframe, Http2_Stream &incstream, const uint8_t *src, const uint8_t *end)
{//解析data帧
    if(incframe._frame_hd.stream_id==0){//流id为0
        return Error_code::PROTOCOL_ERROR;
    }

    if(incstream.m_state!=http2_stream_state::STREAM_OPEN){//流状态异常
        return Error_code::STREAM_CLOSED;
    }

    uint8_t padding=0;
    if(incframe._frame_hd.flags&frameHeader_flag::PADDED){//帧首部带有padded flag
        padding=*src;

        if(padding>=incframe._frame_hd.length){
            return Error_code::PROTOCOL_ERROR;
        }

        src+=sizeof (uint8_t);
    }

    Error_code err=Error_code::NO_ERROR;

//    if(incstream.reserved){

//    }

    if(incframe._frame_hd.flags&frameHeader_flag::END_STREAM){//帧首部带有endstream flag
        incstream.m_state=http2_stream_state::STREAM_LOCAL_HALF_CLOSED;

        //中断数据接收

        incstream.reserved=nullptr;
    }

    return err;
}

Error_code Http2Server::parseHttp2Headers(Frame &incframe, Http2_Stream &incstream, const uint8_t *src, const uint8_t *end)
{//解析header帧
    incstream.m_state=(incframe._frame_hd.flags&frameHeader_flag::END_STREAM)?http2_stream_state::STREAM_REMOTE_HALF_CLOSED
                                                                            :http2_stream_state::STREAM_OPEN;


    uint8_t padding = 0;
    if(incframe._frame_hd.flags&frameHeader_flag::PADDED){//帧首部带有padded标记
        padding=*src;

        if(padding>=incframe._frame_hd.length){
            return Error_code::PROTOCOL_ERROR;
        }

        src+=sizeof (uint8_t);
    }

    if(incframe._frame_hd.flags&frameHeader_flag::PRIORITY){//帧首部带有priority帧
        const uint32_t depend_stream_id=::ntohl(*reinterpret_cast<const uint32_t *>(src))&~(uint32_t(1)<<31);

        std::cout<<"prioriy"<<std::endl;

        src+=sizeof (uint32_t);

        incstream.set_weight(*src);

        src+=sizeof (uint8_t);
    }


    Decoder decoder;
    if(decoder.decode(src,std::size_t(end-src)-padding,incstream,decoding_dynamic_table)==false){//解码头部块
        std::cout<<"decode false"<<std::endl;
        return Error_code::COMPRESSION_ERROR;
    }

    return Error_code::NO_ERROR;

}

Error_code Http2Server::parseHttp2Settings(Frame &incframe, Http2_Stream &incstream, const uint8_t *src, const uint8_t *end)
{//解析setting帧
    if(incframe._frame_hd.stream_id!=0){//streamid应该为0
        return Error_code::PROTOCOL_ERROR;
    }



    if(incframe._frame_hd.length%(sizeof(uint16_t)+sizeof(uint32_t))){//检查长度
        return Error_code::FRAME_SIZE_ERROR;
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

        const uint32_t value=::ntohl(*reinterpret_cast<const uint32_t*>(src));

        src+=sizeof(uint32_t);

        switch(setting){
        case settings_id::SETTINGS_HEADER_TABLE_SIZE :{//设置header表大小
            settings.header_table_size=value;


            break;
        }
        case settings_id::SETTINGS_ENABLE_PUSH:{//推送
            if(value>1){
                return Error_code::PROTOCOL_ERROR;
            }

            settings.enable_push=value;

            break;
        }
        case settings_id::SETTINGS_MAX_CONCURRENT_STREAMS:{//当前流最大数量
            settings.max_concurrent_streams=value;
            break;


        }
        case settings_id::SETTINGS_INITIAL_WINDOW_SIZE:{//流量控制
            if(value>=uint32_t(1)<<31){
                return Error_code::FLOW_CONTROL_ERROR;
            }

            settings.initial_window_size=value;

            break;
        }
        case settings_id::SETTINGS_MAX_FRAME_SIZE:{//设置帧载荷的最大大小
            if(value<(1<<14)||value>=(1<<24)){

                return Error_code::PROTOCOL_ERROR;
            }

            settings.max_frame_size=value;

            break;
        }
        case settings_id::SETTINGS_MAX_HEADER_LIST_SIZE:{//设置header表最大大小
            settings.max_header_list_size=value;

            break;
        }
        default:
            break;
        }

    }

    return Error_code::NO_ERROR;
}

Error_code Http2Server::parseHttp2WindowUpdate(Frame &incframe, Http2_Stream &incstream, const uint8_t *src, const uint8_t *end)
{
    if(incstream.m_state==http2_stream_state::STREAM_LOCAL_RESERVED){
        return Error_code::PROTOCOL_ERROR;
    }else if(incstream.m_state!=http2_stream_state::STREAM_OPEN){
        return Error_code::NO_ERROR;
    }

    if(incframe._frame_hd.length!=sizeof (uint32_t)){
        return Error_code::FRAME_SIZE_ERROR;
    }

    const uint32_t window_size_inc=::ntohl(*reinterpret_cast<const uint32_t*>(src));

    if(window_size_inc==0){
        return Error_code::PROTOCOL_ERROR;
    }else if(window_size_inc>=uint32_t(1)<<31){
        return Error_code::FLOW_CONTROL_ERROR;
    }

    incstream.m_window_size+=window_size_inc;

    return Error_code::NO_ERROR;

}

Error_code Http2Server::parseHttp2rstStream(Frame &incframe, Http2_Stream &incstream, const uint8_t *src, const uint8_t *end)
{
    if (incstream.m_state==http2_stream_state::STREAM_IDLE) {return Error_code::PROTOCOL_ERROR;}

    incstream.m_state=http2_stream_state::STREAM_CLOSE;

    if(incstream.m_stream_id==0){
        return Error_code::PROTOCOL_ERROR;
    }

    if(incframe._frame_hd.length!=sizeof(uint32_t)){
        return Error_code::FRAME_SIZE_ERROR;
    }

    const Error_code error=static_cast<const Error_code>(::ntohl(*reinterpret_cast<const uint32_t*>(src)));

    return Error_code::NO_ERROR;
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

void Http2Server::sendHeaders(boost::asio::ssl::stream<boost::asio::ip::tcp::socket> &socket, std::vector<std::pair<std::string, std::string> > &headers, Http2_Stream& incStream)
{
    std::vector<char> buf;
    buf.reserve(4096);
    buf.resize(FRAME_HEADER_SIZE);

    Encoder::encode(buf,headers,incStream.encoding_dynamic_table);



    const uint32_t frame_size=uint32_t(buf.size()-FRAME_HEADER_SIZE);

    uint8_t flags=frameHeader_flag::END_HEADERS;


    Http2Server::set_frame_header(reinterpret_cast<uint8_t *>(buf.data()),frame_size,frame_type::HTTP2_HEADERS,
                                  (frameHeader_flag)flags,incStream.m_stream_id);

    socket.write_some(boost::asio::buffer(buf,buf.size()));

}

long Http2Server::sendData(boost::asio::ssl::stream<boost::asio::ip::tcp::socket> &socket, const void *src, std::size_t size, Http2_Stream &incStream)
{


    const uint8_t *data=reinterpret_cast<const uint8_t *>(src);


    const ConnectionSettings &setting=incStream.m_connectiondata.client_settings;

    std::vector<uint8_t> buf;
    buf.reserve(setting.max_frame_size+FRAME_HEADER_SIZE);


    long send_size=0;

    while(size!=0)
    {
        std::size_t data_size=setting.max_frame_size<size ? setting.max_frame_size : size;

        const uint8_t padding=Http2Server::getPaddingSize(data_size);//加上的填充字节，增加安全性
        const uint16_t padding_size=padding+sizeof(uint8_t);



        if(padding_size)
        {
            if(setting.max_frame_size<data_size+padding_size)
            {
                data_size=setting.max_frame_size-padding_size;
            }
        }
        const uint32_t frame_size=static_cast<uint32_t>(data_size+padding_size);

        buf.resize(frame_size+FRAME_HEADER_SIZE);

        uint8_t flags=frameHeader_flag::EMPTY;

        if(frame_size!=setting.max_frame_size)//flags加上结束流
        {
            flags |=frameHeader_flag::END_STREAM;
        }


        std::size_t cur=FRAME_HEADER_SIZE;

        if(padding_size)
        {
            flags |=frameHeader_flag::PADDED;
            buf[cur]=padding;
            ++cur;
        }

        Http2Server::set_frame_header(buf.data(),frame_size,frame_type::HTTP2_DATA,(frameHeader_flag)flags,incStream.m_stream_id);

        std::copy(data,data+data_size,buf.data()+cur);

        if(padding)
        {
            std::fill(buf.end()-padding,buf.end(),0);
        }


        const long sendlen=boost::asio::write(socket,boost::asio::buffer(buf,buf.size()));



        if(sendlen<=0){
            send_size=sendlen;
            break;
        }

        data+=data_size;
        send_size+=long(data_size);

        size-=data_size;


    }

    return send_size;
}

void Http2Server::sendWindowUpdate(boost::asio::ssl::stream<boost::asio::ip::tcp::socket> &socket, const Http2_Stream &incstream, const uint32_t size)
{
    std::array<uint8_t,FRAME_HEADER_SIZE+sizeof(uint32_t)>buf;
    uint8_t* addr=buf.data();

    addr=set_frame_header(addr,sizeof(uint32_t),frame_type::HTTP2_WINDOW_UPDATE,frameHeader_flag::EMPTY,incstream.m_stream_id);

    *reinterpret_cast<uint32_t *>(addr)=::htonl(size);

    socket.write_some(boost::asio::buffer(buf.data(),buf.size()));
}

uint8_t Http2Server::getPaddingSize(std::size_t data_size)
{//随机生成若干的填充字节
    if(data_size==0)
         return 0;

    std::random_device rd;
    uint8_t paddings=uint8_t(rd());

    while(paddings>=data_size)
    {
        paddings/=2;
    }
    return paddings;
}



void Http2Server::goAway(boost::asio::ssl::stream<boost::asio::ip::tcp::socket> &socket, ConnectionData &conn, const uint32_t lastStreamId, const Error_code errorcode)
{
    constexpr uint32_t frame_size = sizeof(uint32_t) * 2;

    std::array<uint8_t,FRAME_HEADER_SIZE + frame_size> buf;

    uint8_t *addr=buf.data();

    addr=set_frame_header(addr,frame_size,frame_type::HTTP2_RST_STREAM,frameHeader_flag::EMPTY,0);

    *reinterpret_cast<uint32_t *>(addr) = ::htonl(lastStreamId);

    *reinterpret_cast<uint32_t *>(addr + sizeof(uint32_t) ) = ::htonl(static_cast<const uint32_t>(errorcode));

    //发送数据
    socket.write_some(boost::asio::buffer(buf,buf.size()));

}

void Http2Server::sendRstStream(boost::asio::ssl::stream<boost::asio::ip::tcp::socket> &socket, Http2_Stream &incstream, const Error_code error)
{
    constexpr uint32_t frame_size = sizeof(uint32_t);

    std::array<uint8_t,FRAME_HEADER_SIZE+frame_size>buf;

    uint8_t *addr = buf.data();

    addr = set_frame_header(addr,frame_size,frame_type::HTTP2_RST_STREAM,frameHeader_flag::EMPTY,incstream.m_stream_id);

    *reinterpret_cast<uint32_t *>(addr) = ::htonl(static_cast<const uint32_t>(error));

    socket.write_some(boost::asio::buffer(buf,buf.size()));
}
