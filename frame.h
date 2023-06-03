#ifndef FRAME_H
#define FRAME_H

#include<string>
enum frameHeader_flag{//帧首部标志
    EMPTY=0x0,
    ACK=0x01,
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

constexpr uint32_t FRAME_HEADER_SIZE=9;
constexpr uint32_t MAX_WINDOW_UPDATE=(uint32_t(1)<<31)-1;
constexpr uint32_t MAX_FRAME_SIZE=16384;


class Frame
{
public:
    Frame();
    virtual ~Frame(){}
    void set_max_frame(std::size_t value);
    uint8_t get_frame_type();//返回帧的类型
    uint8_t get_frame_flags();//返回帧的标记

    frame_hd _frame_hd;//帧首部
};

#endif // FRAME_H
