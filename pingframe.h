#ifndef PINGFRAME_H
#define PINGFRAME_H

#include<string>
#include"frame.h"
class PingFrame : public Frame
{
public:
    PingFrame(uint8_t* opaque_data);
private:

    uint8_t* m_opaque_data;//不透明数字字节
};

#endif // PINGFRAME_H
