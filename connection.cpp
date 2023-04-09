#include "connection.h"
#include <iostream>
#include <unistd.h>
#include <string.h>
#include <thread>
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
    //异步读取请求数据，直到空行

    _request=std::make_shared<HttpRequest>();

    boost::asio::async_read_until(socket_, _request->read_buffer, "\r\n\r\n",
                 boost::bind(&Connection::handle_read,
                   shared_from_this(),
                   boost::asio::placeholders::error,
                   boost::asio::placeholders::bytes_transferred));
//    read_buffer.reset(new boost::asio::streambuf());
}

void Connection::handle_read(const boost::system::error_code& error,std::size_t bytes_transferred)
{
    if (!error) {

        //read_buffer->size()不一定与传输的字节相等，有可能有其他数据
        std::size_t total = _request->read_buffer.size();//读到的总大小
        std::cout<<"total:"<<total<<std::endl;

//        std::cout<<_request->read_buffer;
        std::size_t num_additional_bytes = total - bytes_transferred;//其他数据字节数

//        std::cout<<"num_additional_bytes:"<<num_additional_bytes<<std::endl;
        _request->init();//初始化状态
        _request->parse_request();//解析请求
        std::string str=_request->m_path;
        std::cout<<"_request->m_path:"<<str<<std::endl;

       _response=std::make_shared<HttpResponse>("../WebServer/resource",_request->m_path,_request->keepAlive());



        auto it=_request->header_kv.find("Content-Length");
        if(it!=_request->header_kv.end()){//请求中有请求体
            unsigned long long content_length = 0;
            content_length=std::stoull(it->second);
            if(content_length>num_additional_bytes){
                boost::asio::async_read(socket_, _request->read_buffer, boost::asio::transfer_exactly(content_length - num_additional_bytes),
                                        [this](const boost::system::error_code &ec, std::size_t bytes_transferred) {
                    //process body
                    if(!ec) {
                        std::cout<<&_request->read_buffer;

                    }
                    //else error

                });
            }
        }


        _response->buildResponse();


        //异步写响应数据
        boost::asio::async_write(socket_,_response->buffer,
                                  boost::bind(&Connection::handle_write,
                                  shared_from_this(),
                                  boost::asio::placeholders::error
                                  )
                                  );


    }
    else
    {
        std::cout<<"error:"<<error.message()<<std::endl;
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


