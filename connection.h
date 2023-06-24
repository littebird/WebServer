#ifndef CONNECTION_H
#define CONNECTION_H


#include <boost/asio.hpp>
#include <boost/noncopyable.hpp>
#include <memory>
#include <boost/shared_ptr.hpp>
#include <boost/bind.hpp>
#include <boost/asio/ssl.hpp>
#include <gnutls/gnutls.h>
#include "log/access_log.h"
#include "log/log_queue.h"
#include "log/logs.h"
#include "http/httprequest.h"
#include "http/httpresponse.h"
#include "http2server.h"

using ssl_socket=boost::asio::ssl::stream<boost::asio::ip::tcp::socket>;

//此类用于建立连接并获得socket,在进行读写操作
class Connection :public std::enable_shared_from_this<Connection>,
        private boost::noncopyable//不可拷贝
{
public:
    explicit Connection(boost::asio::io_context& io_context,boost::asio::ssl::context& context);
    ssl_socket::lowest_layer_type& socket();//给外部提供ssl低层套接字的引用对象接口
//    boost::asio::ip::tcp::socket& socket();
    void start();//开始处理连接

private:
    void handle_handshake(const boost::system::error_code& error);//ssl握手后回调
    void handle_handshake_h2(const boost::system::error_code& error);
    void handle_read(std::shared_ptr<boost::asio::streambuf> read_buffer,const boost::system::error_code& e,std::size_t bytes_transferred);
    void handle_write(std::shared_ptr<HttpResponse> response,const boost::system::error_code& e);
    void close();//关闭连接
    void set_timeout(long seconds);//设置超时函数
    void cancel_timeout();//取消超时设置
    boost::asio::strand<boost::asio::io_context::executor_type> strand_;//并发事件，使事件能够顺序执行

    std::unique_ptr<ssl_socket>socket_;//ssl_socket套接字
//    std::unique_ptr<boost::asio::ip::tcp::socket>socket_;

    std::unique_ptr<boost::asio::steady_timer> timer_;//定时器
    std::shared_ptr<std::mutex> mutex_;

    std::shared_ptr<Http2Server> h2_;

    std::shared_ptr<HttpRequest> _request;
    std::shared_ptr<HttpResponse> _response;

//    std::atomic<int> count;
};

#endif // CONNECTION_H
