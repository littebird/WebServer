#include "frame.h"

Frame::Frame()
{

}

void Frame::set_max_frame(std::size_t value)
{
    value=std::max(value,static_cast<std::size_t>(16834));
    _frame_hd.length=value;
}

uint8_t Frame::get_frame_type()
{
    return _frame_hd.type;
}

uint8_t Frame::get_frame_flags()
{
    return _frame_hd.flags;
}
