#include "logs.h"

Logs::Logs()
{

}

Logs::~Logs()
{

}
Logs* Logs::get_instance()
{
    static Logs instance;
    return &instance;
}

void* Logs::flush_log_thread(void *args)
{
    Logs::get_instance()->async_write_log();
}

void* Logs::async_write_log()
{
    while(m_log_queue.pop(m_log))
    {
        std::unique_lock<std::mutex> lg(m_mutex);
        std::string data=m_log.merge();
        m_logs.push_back(data);
    }
}

void Logs::init()
{
    std::shared_ptr<std::thread> log_thread(new std::thread(std::bind(&Logs::flush_log_thread,this)));
    log_thread->join();
}
