#ifndef THREADPOOL_H
#define THREADPOOL_H

#include"task.h"
#include<thread>
#include<pthread.h>
#include<mutex>
#include<condition_variable>
#include<iostream>
#include<exception>
#define NUM 6
class ThreadPool
{
private:
    std::vector<Task> _task_queue;//任务队列
    int _num;//线程池中线程的个数
    std::vector<std::thread> threads;
    std::mutex _mutex;       //互斥锁
    std::condition_variable _cond;         //条件变量
    static ThreadPool* _inst; //指向单例对象的static指针
private:
    ThreadPool(int num);//构造函数私有
    //将拷贝构造函数和拷贝赋值函数私有或删除（防拷贝）
    ThreadPool(const ThreadPool&)=delete;
    ThreadPool* operator=(const ThreadPool&)=delete;
    bool IsEmpty();//判断任务队列是否为空
    void LockQueue();//任务队列加锁
    void UnLockQueue(); //任务队列解锁
    void ThreadWait(); //让线程在条件变量下进行等待
    void ThreadWakeUp();//唤醒在条件变量下等待的一个线程
public:
    static ThreadPool* GetInstance();//获取单例对象
    static void* ThreadRoutine(void* arg); //线程的执行例程
    bool InitThreadPool();//初始化线程池
    void PushTask(const Task& task);//将任务放入任务队列
    void PopTask(Task& task);//从任务队列中拿任务
    ~ThreadPool();
};

#endif // THREADPOOL_H
