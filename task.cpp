#include "task.h"
#include<iostream>
#include <boost/asio.hpp>
Task::Task(boost::asio::ip::tcp::socket *sock)
        :_sock(sock)
{

}
Task::Task()
{

    std::cout<<"creat task successful!"<<std::endl;
}

Task::~Task()
{

}
void Task::processon()
{
    //_handler(_sock);//调用回调函数处理任务

}
