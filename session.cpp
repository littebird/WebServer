 #include "session.h"
#include <boost/asio/basic_signal_set.hpp>
#include<iostream>
using boost::asio::ip::tcp;
using boost::asio::buffer;
using boost::system::error_code;
Session::Session(const std::string& address, const std::string& port)
    :
    acceptor_(io_context_),
    new_connection_(new Connection(io_context_)),
    signals_(io_context_)
{
    signals_.add(SIGINT);
     signals_.add(SIGTERM);
#if defined(SIGQUIT)
      signals_.add(SIGQUIT);
#endif // defined(SIGQUIT)
    signals_.async_wait(boost::bind(&Session::handle_stop, this));
    boost::asio::ip::tcp::resolver resolver(io_context_);
     boost::asio::ip::tcp::endpoint endpoint =
       *resolver.resolve(address, port).begin();//ip和端口号
     acceptor_.open(endpoint.protocol());
     acceptor_.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
     acceptor_.bind(endpoint);//绑定套接字
     acceptor_.listen();//监听

    start_accept();
}

void Session::run()
{
  std::vector<std::shared_ptr<std::thread> > threads;//创建装多线程的vector容器
  for (std::size_t i = 0; i < 4; ++i)
  {
      //创建多个线程共享一个io_context,并在每个线程执行io_context.run()
      std::shared_ptr<std::thread> thread(new std::thread(
                                              boost::bind(&boost::asio::io_context::run, &io_context_)));
      threads.push_back(thread);
      //    std::cout<<thread->get_id()<<std::endl;
  }
  //等待创建线程结束
  for (std::size_t i = 0; i < threads.size(); ++i)
      threads[i]->join();
//    io_context_.run();
}

void Session::start_accept()
{
    new_connection_.reset(new Connection(io_context_));//创建新套接字实例
  acceptor_.async_accept(new_connection_->socket(),//异步等待连接
      boost::bind(&Session::handle_accept, this,
        boost::asio::placeholders::error));
}

void Session::handle_accept(const boost::system::error_code& e)
{
  if (!e)
  {
    //异步读取请求数据
    new_connection_->start();
  }
  else
  {
      std::cout<<"error:2"<<e.message()<<std::endl;
  }
  start_accept();
}

void Session::handle_stop()
{
  io_context_.stop();
}

