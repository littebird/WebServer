#ifndef CONNECTION_H
#define CONNECTION_H


#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/bind.hpp>
class Connection :public boost::enable_shared_from_this<Connection>,
        private boost::noncopyable
{
public:
    explicit Connection(boost::asio::io_context& io_context);
    boost::asio::ip::tcp::socket& socket();
    void start();
private:
    void handle_read(const boost::system::error_code& e,std::size_t bytes_transferred);
    void handle_write(const boost::system::error_code& e);
    boost::asio::strand<boost::asio::io_context::executor_type> strand_;
    boost::asio::ip::tcp::socket socket_;
    enum{MAX_length=1024};
    char m_readbuf[MAX_length];//接收到的数据
//    boost::shared_ptr<Connection> connection_ptr;
};

#endif // CONNECTION_H
