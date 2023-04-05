#ifndef SESSION_H
#define SESSION_H

#include <boost/asio.hpp>
#include <string>
#include <vector>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include<thread>
#include <boost/bind.hpp>
#include "connection.h"
class Session: private boost::noncopyable
{
public:
    Session(const std::string& address, const std::string& port);
    void run();
private:
    void start_accept();
    void handle_accept(const boost::system::error_code& e);
    void handle_stop();
    boost::asio::io_context io_context_;
    boost::asio::ip::tcp::acceptor acceptor_;
//     connection_ptr new_connection_;
    boost::shared_ptr<Connection> new_connection_;
};

#endif // SESSION_H
