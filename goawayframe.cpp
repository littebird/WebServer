#include "goawayframe.h"

GoAwayFrame::GoAwayFrame(uint32_t last_stream_id,
                         uint32_t error_code,uint8_t *opaque_data,
                         std::size_t opaque_data_len,uint8_t reserved)
    :m_last_stream_id(last_stream_id),
     m_error_code(error_code),
     m_opaque_data(opaque_data),
     m_opaque_data_len(opaque_data_len),
     m_reserved(reserved)
{

}
