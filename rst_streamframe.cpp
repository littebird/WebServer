#include "rst_streamframe.h"

Rst_StreamFrame::Rst_StreamFrame(uint32_t error_code)
    :m_error_code(error_code)
{

}

uint8_t Rst_StreamFrame::get_frame_type()
{
    return _frame_hd.type;
}
