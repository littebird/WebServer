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

    auto read_buffer=std::make_shared<boost::asio::streambuf>();
    boost::asio::async_read_until(socket_, *read_buffer, "\r\n\r\n",
                 boost::bind(&Connection::handle_read,
                   shared_from_this(),read_buffer,
                   boost::asio::placeholders::error,
                   boost::asio::placeholders::bytes_transferred));


}

void Connection::handle_read(std::shared_ptr<boost::asio::streambuf> read_buffer,const boost::system::error_code& error,std::size_t bytes_transferred)
{
    if (!error) {

        //read_buffer->size()不一定与传输的字节相等，有可能有其他数据
        std::size_t total = read_buffer->size();//读到的总大小

        // 转换到 istream
        std::istream stream(read_buffer.get());

        std::size_t num_additional_bytes = total - bytes_transferred;

        std::cout<<"num_additional_bytes:"<<num_additional_bytes<<std::endl;

        _request=std::make_shared<HttpRequest>();
        _request->parse_request(stream);//解析请求


       _response=std::make_shared<HttpResponse>("../WebServer/resource",_request->m_path,_request->keepAlive());



        auto it=_request->header_kv.find("Content-Length");
        if(it!=_request->header_kv.end()){//请求中有请求体
            unsigned long long content_length = 0;
            content_length=std::stoull(it->second);
            if(content_length>num_additional_bytes){
                boost::asio::async_read(socket_, *read_buffer, boost::asio::transfer_exactly(content_length - num_additional_bytes),
                                        [this,read_buffer](const boost::system::error_code &ec, std::size_t /*bytes_transferred*/) {
                    //process body
                    if(!ec) {
                        std::cout<<read_buffer;

                    }
                    //else error

                });
            }
        }


        _response->buildResponse();

        boost::asio::async_write(socket_,_response->buffer,
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
        socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ignored_ec);
      }

}


