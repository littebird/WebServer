#include "priorityframe.h"

PriorityFrame::PriorityFrame(const frame_hd &frame_hd,uint8_t E,uint32_t stream_dp,uint8_t weight)
    :m_frame_hd(frame_hd),
     m_E(E),
     m_stream_dp(stream_dp),
     m_weight(weight)
{

}
