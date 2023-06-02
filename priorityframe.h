#ifndef PRIORITYFRAME_H
#define PRIORITYFRAME_H

#include"frame.h"
#include<string>
//优先级帧
class PriorityFrame : public Frame
{
public:
    PriorityFrame(uint8_t E,uint32_t stream_dp,uint8_t weight);
private:
    uint8_t m_E;//标识流依赖是唯一地标识符
    uint32_t m_stream_dp;//该数据流依赖的流的标识符
    uint8_t m_weight;//流的优先级权重
};

#endif // PRIORITYFRAME_H
