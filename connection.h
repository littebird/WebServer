#ifndef CONNECTION_H
#define CONNECTION_H


#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/bind.hpp>
#include "httprequest.h"
#include "httpresponse.h"
class Connection :public boost::enable_shared_from_this<Connection>,
        private boost::noncopyable
{
public:
    explicit Connection(boost::asio::io_context& io_context);
    boost::asio::ip::tcp::socket& socket();
    void start();
private:
    void handle_read(std::shared_ptr<boost::asio::streambuf> read_buffer,const boost::system::error_code& e,std::size_t bytes_transferred);
    void handle_write(const boost::system::error_code& e);
    boost::asio::strand<boost::asio::io_context::executor_type> strand_;
    boost::asio::ip::tcp::socket socket_;
    std::shared_ptr<HttpRequest> _request;
    std::shared_ptr<HttpResponse> _response;

//    boost::shared_ptr<Connection> connection_ptr;
};

#endif // CONNECTION_H
