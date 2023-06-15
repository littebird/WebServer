#include "http2_stream.h"

Http2_Stream::Http2_Stream()
    :m_state(STREAM_IDLE)
{

}

Http2_Stream::Http2_Stream(uint32_t streamid,const ConnectionData &conn)
    :m_state(STREAM_IDLE),
    m_stream_id(streamid),
     m_connectiondata(conn)
{

}
void Http2_Stream::stream_init(uint32_t stream_id,http2_stream_state state)
{
    m_stream_id=stream_id;
    m_state=state;
}
void Http2_Stream::set_weight(uint32_t weight)
{
    m_weight=weight;
}
uint32_t Http2_Stream::get_stream_id()
{
    return m_stream_id;
}
bool Http2_Stream::set_send_stream_state(std::shared_ptr<Frame> &item)
{
    switch(m_state)
    {
        case http2_stream_state::STREAM_IDLE:
        {
            if(item->get_frame_type()==0x01)//如果帧为hearder帧
            {
                m_state=STREAM_OPEN;
            }
            else
            {
                return false;
            }
        }
        case http2_stream_state::STREAM_LOCAL_RESERVED:
        {
            if(item->get_frame_type()==0x01)//为hearder帧
            {
                m_state=STREAM_REMOTE_HALF_CLOSED;
            }
            else if(item->get_frame_type()==0x03)//为rst_stream帧
            {
                m_state=STREAM_CLOSE;
            }
            else
            {
                return false;
            }
        }
        case http2_stream_state::STREAM_REMOTE_RESERVED:
        {
            if(item->get_frame_type()==0x03)//rst_stream帧
            {
                m_state=STREAM_CLOSE;
            }
            else
            {
                return false;
            }
        }
        case http2_stream_state::STREAM_OPEN:
        {
            if(item->get_frame_flags()==0x01)//如果该帧的标识为end_stream
            {
                m_state=STREAM_LOCAL_HALF_CLOSED;
            }
            else if(item->get_frame_type()==0x03)//如果为rst_stream帧
            {
                m_state=STREAM_CLOSE;
            }
            else
                   return false;
        }
        case http2_stream_state::STREAM_REMOTE_HALF_CLOSED:
        {
            if(item->get_frame_flags()==0x01)//如果该帧的标识为end_stream
            {
                m_state=STREAM_CLOSE;
            }
        }

    }
    return true;
}
bool Http2_Stream::set_recieve_stream_state(std::shared_ptr<Frame> &item)
{
    switch(m_state)
    {
        case http2_stream_state::STREAM_IDLE:
        {
            if(item->get_frame_type()==0x01)//如果帧为hearder帧
            {
             m_state=STREAM_OPEN;
            }
            else
            {
                return false;
            }
        }
        case http2_stream_state::STREAM_REMOTE_RESERVED:
        {
            if(item->get_frame_type()==0x01)//为hearder帧
            {
                m_state=STREAM_LOCAL_HALF_CLOSED;
            }
            else if(item->get_frame_type()==0x03)//为rst_stream帧
            {
                m_state=STREAM_CLOSE;
            }
            else
            {
                return false;
            }
        }
        case http2_stream_state::STREAM_OPEN:
        {
            if(item->get_frame_flags()==0x01)//如果该帧的标识为end_stream
            {
                m_state=STREAM_REMOTE_RESERVED;
            }
        }
        case http2_stream_state::STREAM_LOCAL_HALF_CLOSED:
        {
            if(item->get_frame_flags()==0x01)//如果该帧的标识为end_stream
            {
                m_state=STREAM_CLOSE;
            }
        }
    }
    return true;
}


