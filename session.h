#ifndef SESSION_H
#define SESSION_H

#include <boost/asio.hpp>
#include <vector>
#include <unordered_set>
#include <boost/noncopyable.hpp>

#include <boost/asio/basic_signal_set.hpp>
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
    void handle_accept(std::shared_ptr<Connection> new_connection,const boost::system::error_code& e);//持续等待连接，进行递归调用
    void handle_stop();//关闭io
    std::string get_password()const{
        return "123456";
    }


    std::shared_ptr<Connection> create_connection(boost::asio::io_context& io_ctx,boost::asio::ssl::context& ctx);//创建连接，返回连接对象
    boost::asio::io_context io_context_;//io上下文
    boost::asio::ssl::context context_;//ssl上下文

    boost::asio::ip::tcp::acceptor acceptor_;//用于指定端口接受连接
    boost::asio::signal_set signals_;
    std::shared_ptr<std::unordered_set<Connection *>>connections_;//Connections
    std::shared_ptr<std::mutex> mutex_;//互斥锁
};

#endif // SESSION_H
