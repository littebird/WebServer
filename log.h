#ifndef LOG_H
#define LOG_H

#include<string>
#include<time.h>
//日志基类
class Log
{
public:
    Log();
    virtual void init(std::string ip_addr,time_t log_time,std::string request_size,std::string file_addr,std::string version,std::string state,int context_length,std::string browser_info);
    virtual void init(time_t log_time,std::string log_level,std::string ip_addr,std::string error_info);
    virtual std::string merge();
    std::string getname()
    {
        return m_name;
    }
public:
    std::string m_name;//标识
private:
    time_t m_log_time; //日志时间
    std::string m_request_size; //请求方法
    std::string m_file_addr; //资源url
    std::string m_ip_addr; //ip地址
    std::string m_state; //返回状态
    std::string m_version;//http协议版本
    int m_context_length; //发送给客户端的总字节数
    std::string m_browser_info; //使用的浏览器信息
    std::string m_log_level;//错误日志等级
     std::string m_error_info;//错误信息
};

#endif // LOG_H
