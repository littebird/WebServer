#ifndef ERROR_LOG_H
#define ERROR_LOG_H

#include<string>
#include<time.h>
#include<mutex>
#include<iostream>
#include"log.h"

class Error_log : public Log//错误日志
{
public:
    Error_log(std::string name);
    ~Error_log();
    void init(std::string log_time,std::string log_level,std::string ip_addr,std::string error_info);
    std::string merge();//合并为字符串
private:
     std::string m_log_time; //日志时间
     std::string m_log_level;//错误日志等级
     std::string m_ip_addr; //客户端ip地址
     std::string m_error_info;//错误信息
};

#endif // ERROR_LOG_H
