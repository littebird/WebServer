#include "log_queue.h"


Log_Queue::Log_Queue()
{

}

Log_Queue::~Log_Queue()
{
    std::unique_lock<std::mutex> lg(m_mutex);
    if(!m_array.empty())
    {
        for(int i=0;i<m_array.size();i++)
        {
            m_array.pop();
        }

    }
}

void Log_Queue::clear()
{
    std::unique_lock<std::mutex> lg(m_mutex);
    if(!m_array.empty())
    {
        for(int i=0;i<m_array.size();i++)
        {
            m_array.pop();
        }

    }
}

bool Log_Queue::empty()
{
    std::unique_lock<std::mutex> lg(m_mutex);
    if(m_array.empty())
    {
        return true;
    }
    else
        return false;
}

int Log_Queue::size()
{
    std::unique_lock<std::mutex> lg(m_mutex);
    return m_array.size();
}

void Log_Queue::push(Log &item)
{
    std::unique_lock<std::mutex> lg(m_mutex);
    m_array.push(item);
    m_cond.notify_one();//唤醒线程
}

bool Log_Queue::pop(Log &item)
{
    std::unique_lock<std::mutex> lg(m_mutex);
    while(m_array.empty())
    {
        m_cond.wait(lg);
        return false;
    }
    item=m_array.front();
    m_array.pop();
    return true;
}
