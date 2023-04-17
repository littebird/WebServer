#ifndef CONNECTION_H
#define CONNECTION_H


#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/bind.hpp>
#include"log.h"
#include"log_queue.h"
#include "httprequest.h"
#include "httpresponse.h"

//此类用于建立连接并获得socket,在进行读写操作
class Connection :public boost::enable_shared_from_this<Connection>,
        private boost::noncopyable
{
public:
    explicit Connection(boost::asio::io_context& io_context);
    boost::asio::ip::tcp::socket& socket();
    void start();//读请求内容
private:
    void handle_read(std::shared_ptr<boost::asio::streambuf> read_buffer,const boost::system::error_code& e,std::size_t bytes_transferred);
    void handle_write(const boost::system::error_code& e);
    boost::asio::strand<boost::asio::io_context::executor_type> strand_;//并发事件，使事件能够顺序执行
    boost::asio::ip::tcp::socket socket_;//socket套接字
    std::shared_ptr<HttpRequest> _request;
    std::shared_ptr<HttpResponse> _response;
};

#endif // CONNECTION_H
