#include "continuationframe.h"

ContinuationFrame::ContinuationFrame(const frame_hd &frame_hd,
                                     const std::unordered_map<std::string,std::string> &header_block)
    :m_frame_hd(frame_hd),
     m_header_block(header_block)
{

}
