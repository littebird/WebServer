#ifndef RST_STREAMFRAME_H
#define RST_STREAMFRAME_H

#include<string>
#include"frame.h"
class Rst_StreamFrame : public Frame
{
public:
    Rst_StreamFrame( uint32_t error_code);
    uint8_t get_frame_type();//返回帧的类型
private:
    uint32_t m_error_code;//错误码
};

#endif // RST_STREAMFRAME_H
