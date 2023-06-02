#include "dataframe.h"

DataFrame::DataFrame(std::size_t padlen,
                     uint8_t padding, const std::string &data)
    :m_padlen(padlen),
     m_padding(padding),
     m_data(data)
{

}

uint8_t DataFrame::get_frame_flag()
{
    return _frame_hd.flag;
}
