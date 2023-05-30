#ifndef DATAFRAME_H
#define DATAFRAME_H

#include<string>
#include"frame.h"
class DataFrame : public Frame
{
public:
    DataFrame(const frame_hd &frame_hd,std::size_t padlen,uint8_t padding,const std::string &data);
    uint8_t get_frame_flag();//返回帧的标记
private:
    frame_hd m_frame_hd;//帧首部
    std::size_t m_padlen;//填充长度
    uint8_t m_padding;//填充字节
    std::string m_data;//请求或响应的数据
};

#endif // DATAFRAME_H
