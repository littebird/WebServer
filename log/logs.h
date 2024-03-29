#ifndef LOGS_H
#define LOGS_H

#include<vector>
#include<string>
#include <boost/noncopyable.hpp>
#include<mutex>
#include<thread>
#include<iostream>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include<fstream>
#include"log.h"
#include"access_log.h"
#include"log_queue.h"
#include"error_log.h"
class Logs:private boost::noncopyable//日志容器类
{
public:
    static Logs *get_instance();//获取单实例对象
    static void *flush_log_thread(void *args);//日志线程起始函数
    void init();//初始化日志
    Log_Queue& logQueue()
    {
        return m_log_queue;
    }

    std::shared_ptr<Log> get_accesslog()
    {
        return accesslog;
    }
    std::shared_ptr<Log> get_errorlog()
    {
        return errorlog;
    }
private:
    Logs();
    ~Logs();
    void *async_write_log();//写入日志
private:
    std::ofstream m_file;//文件写操作
    std::vector<std::string> m_logs;//日志容器
    Log_Queue m_log_queue;//日志队列
    std::shared_ptr<Log> accesslog;//Access_Log类智能指针
    std::shared_ptr<Log> errorlog;//Error_log类智能指针
    std::mutex m_mutex;//互斥锁
};

#endif // LOGS_H
