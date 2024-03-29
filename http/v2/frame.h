#ifndef FRAME_H
#define FRAME_H

#include<string>

enum frameHeader_flag: uint8_t{//帧首部标志
    EMPTY=0x0,
    ACK=0x01,
    END_STREAM=0x01,
    END_HEADERS=0x04,
    PADDED=0x08,
    PRIORITY=0x20,
};
struct frame_hd{//帧首部
    std::size_t length;//帧载荷的长度
    uint32_t stream_id;//流标识符
    uint8_t type;//帧类型，决定帧的格式和语义
    uint8_t flags;//一个特定于帧类型的8位boolean标记保留字段。
    uint8_t reserved;//一个保留的1位字段。
};
enum frame_type: uint8_t{//帧类型
    HTTP2_DATA = 0,
    HTTP2_HEADERS = 0x01,
    HTTP2_PRIORITY = 0x02,
    HTTP2_RST_STREAM = 0x03,
    HTTP2_SETTINGS = 0x04,
    HTTP2_PUSH_PROMISE = 0x05,
    HTTP2_PING = 0x06,
    HTTP2_GOAWAY = 0x07,
    HTTP2_WINDOW_UPDATE = 0x08,
    HTTP2_CONTINUATION = 0x09,
};
enum Error_code{//rst_streamz帧和goaway帧的状态码
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

enum settings_id{//settings id参数定义
    SETTINGS_HEADER_TABLE_SIZE = 0x01,
    SETTINGS_ENABLE_PUSH = 0x02,
    SETTINGS_MAX_CONCURRENT_STREAMS = 0x03,
    SETTINGS_INITIAL_WINDOW_SIZE = 0x04,
    SETTINGS_MAX_FRAME_SIZE = 0x05,
    SETTINGS_MAX_HEADER_LIST_SIZE = 0x06,
    SETTINGS_ENABLE_CONNECT_PROTOCOL = 0x08,
    SETTINGS_NO_RFC7540_PRIORITIES = 0x09
};

constexpr uint32_t FRAME_HEADER_SIZE=9;     //帧首部字节数
constexpr uint32_t MAX_WINDOW_UPDATE=(uint32_t(1)<<31)-1;   //流量控制
constexpr uint32_t MAX_FRAME_SIZE=16384;    //默认最大帧载荷的长度

//帧结构：帧首部+帧有效载荷

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
