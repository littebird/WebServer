#include "threadpool.h"

ThreadPool* ThreadPool::_inst = nullptr;
ThreadPool::ThreadPool(int num = NUM)
    :_num(num)
{

}
bool ThreadPool::IsEmpty()
{
    return _task_queue.empty();
}
void ThreadPool::LockQueue()
{
    _mutex.lock();
}
void ThreadPool::UnLockQueue()
{
    _mutex.unlock();
}
void ThreadPool::ThreadWait()
{
    //可能会出现错误
    std::unique_lock<std::mutex>lk(_mutex,std::adopt_lock);
    _cond.wait(lk);
}
void ThreadPool::ThreadWakeUp()
{
    _cond.notify_one();
}

ThreadPool* ThreadPool::GetInstance()
{
    static std::mutex mtx;
    //双检查加锁
    if(_inst==nullptr)
    {
        mtx.lock();
        if(_inst==nullptr)
        {
            //创建单例线程池对象并初始化
            _inst = new ThreadPool();
            _inst->InitThreadPool();
        }
        mtx.unlock();
    }
    return _inst;
}
//线程的起始函数
void* ThreadPool::ThreadRoutine(void* arg)
{
    ThreadPool* tp = (ThreadPool*)arg;
    while(true){
        tp->LockQueue(); //加锁
        while(tp->IsEmpty()){
            //任务队列为空，线程进行wait
            tp->ThreadWait();
        }
        Task task;
        tp->PopTask(task); //获取任务
        tp->UnLockQueue(); //解锁

        task.processon(); //处理任务
    }
}


bool ThreadPool::InitThreadPool()
{
    //创建线程池中的若干线程
    try {
        for(unsigned i=0;i<_num;i++)
        {
            std::cout<<"create the"<<i<<"thread"<<std::endl;
            threads.push_back(
                        std::thread(ThreadPool::ThreadRoutine,this));
            threads[i].detach();//线程分离

        }
    } catch (...) {
        throw std::exception();
        return false;
    }
    return true;
}

//将任务放入任务队列
void ThreadPool::PushTask(const Task& task)
{
    LockQueue();    //加锁
    _task_queue.push_back(task); //将任务推入任务队列
    UnLockQueue();  //解锁
    ThreadWakeUp(); //唤醒一个线程进行任务处理
}

//从任务队列中拿任务
void ThreadPool::PopTask(Task& task)
{
    //获取任务
    task = _task_queue.front();
    _task_queue.pop_back();
}

ThreadPool::~ThreadPool()
{

}


