#ifndef CONNECTION_H
#define CONNECTION_H


#include <boost/asio.hpp>
#include <boost/noncopyable.hpp>
#include <memory>
#include <boost/bind.hpp>
#include"log.h"
#include"log_queue.h"
#include"logs.h"
#include "httprequest.h"
#include "httpresponse.h"

//此类用于建立连接并获得socket,在进行读写操作
class Connection :public std::enable_shared_from_this<Connection>,
        private boost::noncopyable//不可拷贝
{
public:
    explicit Connection(boost::asio::io_context& io_context);
    boost::asio::ip::tcp::socket& socket();//给外部提供套接字的引用对象接口
    void start();//读取请求内容

private:
    void handle_read(std::shared_ptr<boost::asio::streambuf> read_buffer,const boost::system::error_code& e,std::size_t bytes_transferred);
    void handle_write(std::shared_ptr<HttpResponse> response,const boost::system::error_code& e);
    void close();//关闭连接
    void set_timeout(long seconds);//设置超时函数
    void cancel_timeout();//取消超时设置
    boost::asio::strand<boost::asio::io_context::executor_type> strand_;//并发事件，使事件能够顺序执行

    std::unique_ptr<boost::asio::ip::tcp::socket>socket_;//socket套接字
    std::unique_ptr<boost::asio::steady_timer> timer_;//定时器
    std::shared_ptr<std::mutex> mutex_;


    std::shared_ptr<HttpRequest> _request;
    std::shared_ptr<HttpResponse> _response;
};

#endif // CONNECTION_H
