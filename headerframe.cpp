#include "headerframe.h"

HeaderFrame::HeaderFrame(std::size_t padlen,
                         uint8_t E, uint32_t stream_dp,
                         headers_category cat, uint8_t weight,uint32_t padding,
                         const std::unordered_map<std::string,std::string> &header_block)
    :m_padlen(padlen),
     m_E(E),
     m_stream_dp(stream_dp),
     m_cat(cat),
     m_weight(weight),
     m_padding(padding),
     m_header_block(header_block)
{

}
void HeaderFrame::set_category(headers_category cat)
{
   m_cat=cat;
}
uint8_t HeaderFrame::get_frame_type()
{
    return _frame_hd.type;
}
uint8_t HeaderFrame::get_frame_flag()
{
    return _frame_hd.flag;
}
