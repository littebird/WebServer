#include "connection.h"
#include <iostream>
#include <unistd.h>
#include <string.h>
#include <thread>
Connection::Connection(boost::asio::io_context& io_context)
  : strand_(boost::asio::make_strand(io_context)),
    socket_(new boost::asio::ip::tcp::socket(io_context))
{

}

boost::asio::ip::tcp::socket& Connection::socket()
{
  return *socket_;
}

void Connection::start()
{

//    std::cout<<std::this_thread::get_id()<<std::endl;

//    auto time=std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());//获得当前时间并to time_t
//    auto curTime(ctime(&time));

    set_timeout(5);
    //异步读取请求数据，直到空行
    auto read_buffer=std::make_shared<boost::asio::streambuf>();

    boost::asio::async_read_until(*socket_, *read_buffer, "\r\n\r\n",
                 boost::bind(&Connection::handle_read,
                   shared_from_this(),read_buffer,
                   boost::asio::placeholders::error,
                   boost::asio::placeholders::bytes_transferred));
//    read_buffer.reset(new boost::asio::streambuf());
}

void Connection::handle_read(std::shared_ptr<boost::asio::streambuf> read_buffer, const boost::system::error_code& error, std::size_t bytes_transferred)
{
    cancel_timeout();
    if (!error) {
        //read_buffer->size()不一定与传输的字节相等，有可能有附加的数据
        std::size_t total = read_buffer->size();//读到的总大小
        //        std::cout<<"total:"<<total<<std::endl;
        std::size_t bytes = total - bytes_transferred;//附加数据字节数
//        std::cout<<"bytes:"<<bytes<<std::endl;

        std::ostream os(read_buffer.get());
        os<<"\n";//在读缓冲区尾中加入换行符作结束标记,便于request解析

        std::istream is(read_buffer.get());

        _request=std::make_shared<HttpRequest>();

        _request->init();//初始化状态
        if(_request->parse_request(is)==HttpRequest::BAD_REQUEST)//解析请求
            _request->errorRes();

//        std::string str=_request->request_body();
//        std::cout<<str<<std::endl;

        //Log log;
        _response=std::make_shared<HttpResponse>("../WebServer/resource",_request->path(),_request->keepAlive());


        _response->buildResponse();

        //异步写响应数据
        boost::asio::async_write(*socket_,_response->buffer,
                                 boost::bind(&Connection::handle_write,
                                             shared_from_this(),
                                             boost::asio::placeholders::error
                                             )
                                 );
    }
}

void Connection::handle_write(const boost::system::error_code& e)
{
    if (!e)
      {
        //to do process
      }

}

void Connection::close()
{
    boost::system::error_code ignored_ec;
    //关闭双方连接
    socket_->shutdown(boost::asio::ip::tcp::socket::shutdown_both, ignored_ec);
    socket_->close(ignored_ec);
}

void Connection::set_timeout(long seconds) {
    if(seconds == 0) {
        timer_ = nullptr;
        return;
    }

    timer_ = std::unique_ptr<boost::asio::steady_timer>(new boost::asio::steady_timer((boost::asio::io_context&)(socket_)->get_executor().context()));
    timer_->expires_from_now(std::chrono::seconds(seconds));
    auto self = this->shared_from_this();
    timer_->async_wait([self](const boost::system::error_code &ec) {
        if(!ec)
            self->close();
    });
}

void Connection::cancel_timeout() {
    if(timer_) {
        boost::system::error_code ec;
        timer_->cancel(ec);
    }
}
