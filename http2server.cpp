#include "http2server.h"

Http2Server::Http2Server()
{

}

error_code Http2Server::parseHttp2Date(Frame &incframe, Http2_Stream &incstream, const uint8_t *src, const uint8_t *end)
{
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
{
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
{

}
