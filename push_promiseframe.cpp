#include "push_promiseframe.h"

Push_PromiseFrame::Push_PromiseFrame(const frame_hd &frame_hd,std::size_t padlen,
                                     nv *nva,std::size_t nvlen,
                                     uint32_t promise_stream_id,uint8_t reserved)
    :m_frame_hd(frame_hd),
     m_padlen(padlen),
     m_nva(nva),
     m_nvlen(nvlen),
     m_promise_stream_id(promise_stream_id),
     m_reserved(reserved)
{

}
