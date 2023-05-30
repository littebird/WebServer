#ifndef PINGFRAME_H
#define PINGFRAME_H

#include<string>
#include"frame.h"
class PingFrame : public Frame
{
public:
    PingFrame(const frame_hd &frame_hd,const uint8_t &opaque_data);
private:
    frame_hd m_frame_hd;//帧首部
    uint8_t m_opaque_data[8];//不透明数字字节
};

#endif // PINGFRAME_H
