#include "http2_stream.h"

Http2_Stream::Http2_Stream()
    :m_state(STREAM_IDLE)
{

}
void Http2_Stream::stream_init(uint32_t stream_id,http2_stream_state state)
{
    m_stream_id=stream_id;
    m_state=state;
}
int Http2_Stream::stream_attach_item()
{
    return 0;
}
int Http2_Stream::stream_detach_item()
{
    return 0;
}
bool Http2_Stream::set_send_stream_state(std::shared_ptr<std::string> item)
{
    switch(m_state)
    {
        case http2_stream_state::STREAM_IDLE:
        {
            if(1)//如果帧为hearder帧
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
            if(1)//为hearder帧
            {
                m_state=STREAM_REMOTE_HALF_CLOSED;
            }
            else if(1)//为rst_stream帧
            {
                m_state=STREAM_CLOSED;
            }
            else
            {
                return false;
            }
        }
        case http2_stream_state::STREAM_REMOTE_RESERVED:
        {
            if(1)//rst_stream帧
            {
                m_state=STREAM_CLOSED;
            }
            else
            {
                return false;
            }
        }
        case http2_stream_state::STREAM_OPEN:
        {
            if(1)//如果该帧的标识为end_stream
            {
                m_state=STREAM_LOCAL_HALF_CLOSED;
            }
            else if(1)//如果为rst_stream帧
            {
                m_state=STREAM_CLOSED;
            }
        }

    }
    return true;
}
bool Http2_Stream::set_recieve_stream_state(std::shared_ptr<std::string> item)
{
    switch(m_state)
    {
        case http2_stream_state::STREAM_IDLE:
        {
            if(1)//如果帧为hearder帧
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
            if(1)//为hearder帧
            {
                m_state=STREAM_LOCAL_HALF_CLOSED;
            }
            else if(1)//为rst_stream帧
            {
                m_state=STREAM_CLOSED;
            }
            else
            {
                return false;
            }
        }
        case http2_stream_state::STREAM_OPEN:
        {
            if(1)//如果该帧的标识为end_stream
            {
                m_state=STREAM_REMOTE_RESERVED;
            }
        }
        case http2_stream_state::STREAM_LOCAL_HALF_CLOSED:
        {
            if(1)//如果该帧的标识为end_stream
            {
                m_state=STREAM_CLOSED;
            }
            else if(1)//如果为rst_stream帧
            {
             m_state=STREAM_CLOSED;
            }
        }
    }

}


