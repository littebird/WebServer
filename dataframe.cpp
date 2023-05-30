#include "dataframe.h"

DataFrame::DataFrame(const frame_hd &frame_hd,std::size_t padlen,
                     uint8_t padding,const std::string &data)
    :m_frame_hd(frame_hd),
     m_padlen(padlen),
     m_padding(padding),
     m_data(data)
{

}

uint8_t DataFrame::get_frame_flag()
{
    return m_frame_hd.flag;
}
