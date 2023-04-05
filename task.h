#ifndef TASK_H
#define TASK_H
#include <boost/asio.hpp>

using boost::asio::ip::tcp;
class Task
{
private:
    boost::asio::ip::tcp::socket *_sock;//通信的套接字
    //CallBack _handler; //回调函数
public:
    Task(boost::asio::ip::tcp::socket *sock);
    Task();
    void processon();//处理任务
    ~Task();
};

#endif // TASK_H
