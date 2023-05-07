#include "error_log.h"

Error_log::Error_log(std::string name)
{
    m_name=name;
}
Error_log::~Error_log()
{
}

void Error_log::init(std::string log_time,std::string log_level,std::string ip_addr,std::string error_info)
{
    m_log_time=log_time;
    m_log_level=log_level;
    m_ip_addr=ip_addr;
    m_error_info=error_info;
}
std::string Error_log::merge()
{
    std::string merged;
    merged="["+m_log_time+"] ["+m_log_level+"] [client"+m_ip_addr+"] error_infomations:"+m_error_info;
    return merged;
}
