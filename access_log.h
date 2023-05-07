#ifndef ACCESS_LOG_H
#define ACCESS_LOG_H

#include<string>
#include<time.h>
#include<mutex>
#include"log.h"
class Access_Log : public Log//单个日志类
{
public:
    Access_Log(std::string name);
    ~Access_Log();
    void init(std::string ip_addr,time_t log_time,std::string request_size,std::string file_addr,std::string version,std::string state,int context_length,std::string browser_info);
    std::string merge();//合并为字符串
private:
    time_t m_log_time; //日志时间
    std::string m_request_size; //请求方法
    std::string m_file_addr; //资源url
    std::string m_ip_addr; //ip地址
    std::string m_state; //返回状态
    std::string m_version;//http协议版本
    int m_context_length; //发送给客户端的总字节数
    std::string m_browser_info; //使用的浏览器信息
};

#endif // ACCESS_LOG_H
