#ifndef LOGS_H
#define LOGS_H

#include<vector>
#include<string>
#include <boost/noncopyable.hpp>
#include<mutex>
#include<thread>
#include<iostream>
#include <boost/bind.hpp>
#include"log.h"
#include"log_queue.h"
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

//    void coutinfo()
//    {
//        std::unique_lock<std::mutex> lg(m_mutex);
//        for(auto it:m_logs)
//        {
//            std::cout<<it<<std::endl;
//        }
//    }
//    int size()
//    {
//        return m_logs.size();
//    }
    Log& getlog()
    {
        return m_log;
    }
private:
    Logs();
    ~Logs();
    void *async_write_log();//写入日志
private:
    std::vector<std::string> m_logs;//日志容器
    Log_Queue m_log_queue;//日志队列
    Log m_log;//单个日志
    std::mutex m_mutex;//互斥锁
};

#endif // LOGS_H
