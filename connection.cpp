#include "connection.h"
#include <iostream>
#include <unistd.h>
#include<string.h>
#include<thread>
Connection::Connection(boost::asio::io_context& io_context)
  : strand_(boost::asio::make_strand(io_context)),
    socket_(io_context)
{
}

boost::asio::ip::tcp::socket& Connection::socket()
{
  return socket_;
}

void Connection::start()
{
    //异步读取请求数据
  socket_.async_read_some(boost::asio::buffer(m_readbuf,MAX_length),
      boost::bind(&Connection::handle_read, shared_from_this(),
        boost::asio::placeholders::error,
        boost::asio::placeholders::bytes_transferred));
}

void Connection::handle_read(const boost::system::error_code& error,std::size_t bytes_transferred)
{
    if(!error){
        std::cout<<m_readbuf<<std::endl;
        std::cout<<"successeful!\n";
        std::cout<<std::this_thread::get_id()<<std::endl;
        std::string reply="HTTP/1.1 200 OK\r\n";
        reply +="Server:httpserver\r\n";
        reply +="Content-type:text-html\r\n";
        reply +="\r\n";//空行
        reply +="hello.c";
        memcpy(m_readbuf,reply.c_str(),reply.size());
        //异步写响应数据
        boost::asio::async_write(socket_,boost::asio::buffer(m_readbuf,reply.size()),
                                  boost::bind(&Connection::handle_write,
                                  shared_from_this(),
                                  boost::asio::placeholders::error
                                  )
                                  );


    }
    else
    {
        std::cout<<"efdes"<<std::endl;
    }
}

void Connection::handle_write(const boost::system::error_code& e)
{
    if (!e)
      {
        boost::system::error_code ignored_ec;
        //关闭双方连接
        socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ignored_ec);
      }

}


