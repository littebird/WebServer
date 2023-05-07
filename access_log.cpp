#include "access_log.h"
#include<iostream>
Access_Log::Access_Log(std::string name)
{
    m_name=name;
}
Access_Log::~Access_Log()
{

}
void Access_Log::init(std::string ip_addr,std::string log_time,std::string request_size,std::string file_addr,std::string version,std::string state,int context_length,std::string browser_info)
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
std::string Access_Log::merge()
{
    std::string merged;
    merged=m_ip_addr+" - - ["+m_log_time+"]"+ " \" "+m_request_size+" "+m_file_addr+" "+m_version+"\" "+m_state+" "+std::to_string(m_context_length)+" - "+m_browser_info;
    return merged;
}
