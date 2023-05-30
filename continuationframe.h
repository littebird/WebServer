#ifndef CONTINUATIONFRAME_H
#define CONTINUATIONFRAME_H

#include<unordered_map>
#include"frame.h"

class ContinuationFrame :public Frame
{
public:
    ContinuationFrame(const frame_hd &frame_hd,const std::unordered_map<std::string,std::string> &header_block);
private:
    frame_hd m_frame_hd;//帧首部
    std::unordered_map<std::string,std::string> m_header_block;
};

#endif // CONTINUATIONFRAME_H
