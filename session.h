#ifndef SESSION_H
#define SESSION_H

#include <boost/asio.hpp>
#include <string>
#include <vector>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <thread>
#include <boost/bind.hpp>
#include "connection.h"

class Session: private boost::noncopyable//不可拷贝类
{
public:
    Session(const std::string& address, const std::string& port);
    void run();//创建多线程，并分别执行io_context的run函数
private:
    void start_accept();//异步等待连接
    void handle_accept(const boost::system::error_code& e);//持续等待连接，进行递归调用
    void handle_stop();//关闭io
    boost::asio::io_context io_context_;//io服务
    boost::asio::ip::tcp::acceptor acceptor_;//用于指定端口接受连接
    boost::shared_ptr<Connection> new_connection_;//Connection类的智能指针
};

#endif // SESSION_H
