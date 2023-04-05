#include "session.h"

#include<iostream>
using boost::asio::ip::tcp;
using boost::asio::buffer;
using boost::system::error_code;
Session::Session(const std::string& address, const std::string& port)
    :
    acceptor_(io_context_),
    new_connection_(new Connection(io_context_))
{
    boost::asio::ip::tcp::resolver resolver(io_context_);
     boost::asio::ip::tcp::endpoint endpoint =
       *resolver.resolve(address, port).begin();
     acceptor_.open(endpoint.protocol());
     acceptor_.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
     acceptor_.bind(endpoint);
     acceptor_.listen();

    start_accept();
}

void Session::run()
{
//    io_context_.run();
  std::vector<boost::shared_ptr<std::thread> > threads;
  for (std::size_t i = 0; i < 4; ++i)
  {
    boost::shared_ptr<std::thread> thread(new std::thread(
          boost::bind(&boost::asio::io_context::run, &io_context_)));
    threads.push_back(thread);
//    std::cout<<std::this_thread::get_id()<<std::endl;
  }
  for (std::size_t i = 0; i < threads.size(); ++i)
    threads[i]->join();
}

void Session::start_accept()
{
    new_connection_.reset(new Connection(io_context_));
  acceptor_.async_accept(new_connection_->socket(),
      boost::bind(&Session::handle_accept, this,
        boost::asio::placeholders::error));
}

void Session::handle_accept(const boost::system::error_code& e)
{
  if (!e)
  {
    new_connection_->start();
  }

  start_accept();
}

void Session::handle_stop()
{
  io_context_.stop();
}

