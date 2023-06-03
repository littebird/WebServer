#ifndef FRAME_H
#define FRAME_H

#include<string>
enum frameHeader_flag{//头部帧标志
    EMPTY=0,
    END_STREAM=0x01,
    END_HEADERS=0x04,
    PADDED=0x08,
    PRIORIY=0x20,
};
struct frame_hd{//帧首部
    std::size_t length;//帧载荷的长度
    uint32_t stream_id;//流标识符
    uint8_t type;//帧类型，决定帧的格式和语义
    uint8_t flags;//一个特定于帧类型的8位boolean标记保留字段。
    uint8_t reserved;//一个保留的1位字段。
};
enum frame_type{//帧类型
    NGHTTP2_DATA = 0,
    NGHTTP2_HEADERS = 0x01,
    NGHTTP2_PRIORITY = 0x02,
    NGHTTP2_RST_STREAM = 0x03,
    NGHTTP2_SETTINGS = 0x04,
    NGHTTP2_PUSH_PROMISE = 0x05,
    NGHTTP2_PING = 0x06,
    NGHTTP2_GOAWAY = 0x07,
    NGHTTP2_CONTINUATION = 0x09,
};
enum error_code{//rst_streamz帧和goaway帧的状态码
    NO_ERROR = 0x00,
    PROTOCOL_ERROR = 0x01,
    INTERNAL_ERROR = 0x02,
    FLOW_CONTROL_ERROR = 0x03,
    SETTINGS_TIMEOUT = 0x04,
    STREAM_CLOSED = 0x05,
    FRAME_SIZE_ERROR = 0x06,
    REFUSED_STREAM = 0x07,
    CANCEL = 0x08,
    COMPRESSION_ERROR = 0x09,
    CONNECT_ERROR = 0x0a,
    ENHANCE_YOUR_CALM = 0x0b,
    INADEQUATE_SECURITY = 0x0c,
};
//struct nv{//键值对结构
//    uint8_t *name;
//    uint8_t *value;
//    std::size_t namelen;
//    std::size_t valuelen;
//    uint8_t flags;//标记内容在nv_flag
//};
//enum nv_flag{//键值对标记
//    NV_FLAG_NONE = 0,
//    NV_FLAG_NO_INDEX = 0x01,
//    NV_FLAG_NO_COPY_NAME = 0x02,
//    NO_COPY_VALUE = 0x04
//};
class Frame
{
public:
    Frame();
    virtual uint8_t get_frame_type()=0;//返回帧的类型
    virtual uint8_t get_frame_flag()=0;//返回帧的标记
protected:
    frame_hd _frame_hd;//帧首部
};

#endif // FRAME_H
