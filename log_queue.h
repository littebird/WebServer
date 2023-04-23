#ifndef LOG_QUEUE_H
#define LOG_QUEUE_H

#include<mutex>
#include<condition_variable>
#include"log.h"
#include<queue>
#include<iostream>
class Log_Queue//日志队列类
{
public:
    Log_Queue();
    ~Log_Queue();
    void clear();//清空队列
    bool empty();//判断队列是否为空
    int size();//返回队列长度
    void push(Log &item);//添加元素
    bool pop(Log &item);//取出队头元素
private:
    std::mutex m_mutex; // 互斥锁
    std::condition_variable m_cond;// 条件变量
    std::queue<Log,std::deque<Log>> m_array; // 循环数组实现队列
};

#endif // LOG_QUEUE_H
