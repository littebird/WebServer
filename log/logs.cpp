#include "logs.h"

Logs::Logs()
{
    accesslog=std::make_shared<Access_Log>("accesslog");
    errorlog=std::make_shared<Error_log>("errorlog");
}

Logs::~Logs()
{
      m_file.close();
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
    Log item;
    while(m_log_queue.pop(item))//获取队列数据
    {
        std::unique_lock<std::mutex> lg(m_mutex);
        if(accesslog->getname()==item.getname())//判断是否为access日志
        {
            //ios::out(为输出写而打开文件) ios::app(所有输出附加在文件末尾)
            m_file.open("../WebServer/resource/accesslogfiles.txt",std::ios::out|std::ios::app);
            std::string data=accesslog->merge();
            m_file<<data<<"\n";//数据写入文件
            m_file.close();//关闭文件
        }
        else
        {
            m_file.open("../WebServer/resource/errorlogfiles.txt",std::ios::out|std::ios::app);
            std::string data=errorlog->merge();
            m_file<<data<<"\n";
            m_file.close();
        }
    }
}

void Logs::init()
{
    std::shared_ptr<std::thread> log_thread(new std::thread(std::bind(&Logs::flush_log_thread,this)));//创建日志线程
    log_thread->detach();//线程分离
}
