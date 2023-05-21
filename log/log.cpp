#include "log.h"
#include<iostream>
Log::Log()
{

}
void Log::init(std::string ip_addr,std::string log_time,std::string request_size,std::string file_addr,std::string version,std::string state,int context_length,std::string browser_info)
{
    m_ip_addr=ip_addr;
    m_log_time=log_time;
    m_request_size=request_size;
    m_file_addr=file_addr;
    m_version=version;
    m_state=state;
    m_context_length=context_length;
    m_browser_info=browser_info;
}
std::string Log::merge()
{
    std::string merged;
    merged=m_ip_addr+" - - ["+m_log_time+"]"+ " \" "+m_request_size+" "+m_file_addr+" "+m_version+"\" "+m_state+" "+std::to_string(m_context_length)+" - "+m_browser_info;
    return merged;
}
void Log::init(std::string log_time,std::string log_level,std::string ip_addr,std::string error_info)
{
    m_log_time=log_time;
    m_log_level=log_level;
    m_ip_addr=ip_addr;
    m_error_info=error_info;
}
