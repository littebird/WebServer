#include "headerframe.h"

HeaderFrame::HeaderFrame(const std::string &frame_hd,std::size_t padlen,
                         headerframe_flag flag,uint8_t E,
                         uint32_t stream_dp,headers_category cat,
                         uint8_t weight,uint32_t padding,
                         uint32_t stream_id)
    :m_frame_hd{frame_hd},
     m_padlen(padlen),
     m_flag(flag),
     m_E(E),
     m_stream_dp(stream_dp),
     m_cat(cat),
     m_weight(weight),
     m_padding(padding),
     m_stream_id(stream_id)
{

}
void HeaderFrame::set_frame_flag(headerframe_flag flag)
{
    m_flag=flag;
}
void HeaderFrame::set_category(headers_category cat)
{
   m_cat=cat;
}
headerframe_flag HeaderFrame::get_flag()
{
    return m_flag;
}
