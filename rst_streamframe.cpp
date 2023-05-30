#include "rst_streamframe.h"

Rst_StreamFrame::Rst_StreamFrame(const frame_hd &frame_hd, uint32_t error_code)
    :m_frame_hd(frame_hd),
      m_error_code(error_code)
{

}

uint8_t Rst_StreamFrame::get_frame_type()
{
    return m_frame_hd.type;
}
