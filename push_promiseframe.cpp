#include "push_promiseframe.h"

Push_PromiseFrame::Push_PromiseFrame(std::size_t padlen,
                                     uint32_t promise_stream_id, uint8_t reserved)
    :m_padlen(padlen),
     m_promise_stream_id(promise_stream_id),
     m_reserved(reserved)
{

}
