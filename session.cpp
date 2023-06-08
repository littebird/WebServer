 #include "session.h"
#include <boost/asio/basic_signal_set.hpp>
#include<iostream>

using boost::asio::ip::tcp;
using boost::asio::buffer;
using boost::system::error_code;
Session::Session(const std::string& address, const std::string& port)
    :context_(boost::asio::ssl::context::sslv23),
    acceptor_(io_context_),
    signals_(io_context_),
    mutex_(new std::mutex()),
    connections_(new std::unordered_set<Connection *>())
{
    signals_.add(SIGINT);
    signals_.add(SIGTERM);
#if defined(SIGQUIT)
    signals_.add(SIGQUIT);
#endif // defined(SIGQUIT)

    signals_.async_wait(boost::bind(&Session::handle_stop, this));

    //设置ssl上下文
    context_.set_options(boost::asio::ssl::context::default_workarounds|
                         boost::asio::ssl::context::no_sslv2|
                         boost::asio::ssl::context::single_dh_use);
    context_.set_password_callback(boost::bind(&Session::get_password,this));
    context_.use_certificate_chain_file("/root/cert.pem");//证书文件
    context_.use_private_key_file("/root/privkey.pem",boost::asio::ssl::context::pem);//私钥文件


    boost::asio::ip::tcp::resolver resolver(io_context_);
     boost::asio::ip::tcp::endpoint endpoint =
       *resolver.resolve(address, port).begin();//ip和端口号
     acceptor_.open(endpoint.protocol());
     acceptor_.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true)); // 设置地址可重用
     acceptor_.bind(endpoint);//绑定套接字
     acceptor_.listen();//监听

     start_accept();
}

void Session::run()//线程池实现
{

  std::vector<std::shared_ptr<std::thread> > threads;//线程池
  unsigned const thread_count=std::thread::hardware_concurrency();//硬件所支持的并发线程数目
  for (unsigned i = 0; i <thread_count ; ++i)
  {
      //创建多个线程共享一个io_context,并在每个线程执行io_context.run()，执行io事件队列处理
      std::shared_ptr<std::thread> thread(new std::thread(
                                              boost::bind(&boost::asio::io_context::run, &io_context_)));

      threads.push_back(thread);
//          std::cout<<thread->get_id()<<std::endl;
  }

  //等待创建线程结束
  for (auto& th:threads)
      th->join();


}

void Session::start_accept()
{

    auto new_connection=create_connection(io_context_,context_);
    acceptor_.async_accept(new_connection->socket(),//异步等待连接
                           boost::bind(&Session::handle_accept, this,
                                       new_connection,
                                       boost::asio::placeholders::error));

}

void Session::handle_accept(std::shared_ptr<Connection> new_connection,const boost::system::error_code& e)
{
  if (!e)
  {
    //异步读取请求数据
    new_connection->start();
  }
  start_accept();
}

void Session::handle_stop()
{
      io_context_.stop();

}

std::shared_ptr<Connection> Session::create_connection(boost::asio::io_context& io_ctx,boost::asio::ssl::context& ctx)
{

    auto connections=this->connections_;
    auto mutex_con=this->mutex_;
    auto connection=std::shared_ptr<Connection>(new Connection(io_ctx,ctx),[connections,mutex_con](Connection* connection){
        //用lambda自定义connection智能指针释放资源方式,删除器
        {//缩小锁的粒度
            std::unique_lock<std::mutex> lock(*mutex_con);
            auto it=connections->find(connection);
            if(it!=connections->end()){
                connections->erase(it);//从connections中删除
            }
        }

        delete connection;
    });

    {
        std::unique_lock<std::mutex> lock(*mutex_);
        connections_->emplace(connection.get());
    }
    return connection;
}

