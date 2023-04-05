//a webserver by c++ implement
#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/asio/steady_timer.hpp>
#include <regex>
#include <boost/asio/detail/io_uring_socket_sendto_op.hpp>

#include <array>
using boost::asio::ip::tcp;
using boost::asio::buffer;
using boost::system::error_code;


class Http_server: public std::enable_shared_from_this<Http_server>
{
public:
    explicit Http_server(boost::asio::io_context& io_context)
        :m_io_context(io_context),m_acceptor(io_context,tcp::endpoint(tcp::v4(),8888)),m_socket(new tcp::socket(io_context))
    {
            start_accept();

    }


    void start_accept(){

        std::cout<<"wait for client request...\n";
        m_acceptor.listen(boost::asio::socket_base::max_connections);
        m_acceptor.async_accept(*m_socket,boost::bind(&Http_server::handle_accept,this,m_socket,boost::asio::placeholders::error/*占位符*/));  //异步等待连接

    }

    void handle_accept(std::shared_ptr<tcp::socket> _socket,const boost::system::error_code& error){


        //after accept do something
        if(error){//错误码检查
            std::cout<<"error!\n";
            return;
        }

        _socket->async_read_some(buffer(m_readbuf,MAX_length),boost::bind(&Http_server::handle_read,this,_socket,boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));

//        auto read_buffer=std::make_shared<boost::asio::streambuf>();
//        boost::asio::async_read_until(*_socket, *read_buffer, "\r\n\r\n",
//                          [this, _socket, read_buffer](const boost::system::error_code &ec,std::size_t bytes_transferred) {
//                              if (!ec) {

//                                  std::size_t total = read_buffer->size();

//                                  std::cout<<"total:"<<total<<std::endl;
//                                  std::cout<<"bytes_transferred:"<<bytes_transferred<<std::endl;
//                                  // 转换到 istream
//                                  std::istream stream(read_buffer.get());

//                                  std::size_t num_additional_bytes = total - bytes_transferred;

//                                  std::cout<<"num_additional_bytes:"<<num_additional_bytes<<std::endl;


//                                    std::string line;
//                                  while(getline(stream,line)){


//                                      line.pop_back();
//                                      std::cout<<line<<std::endl;

//                                  }

//                                 }

//                                  });



    }

    void handle_read(std::shared_ptr<tcp::socket> _socket,const boost::system::error_code& error,std::size_t bytes_transferred){
        if(!error){
            std::cout<<m_readbuf<<std::endl;
            std::cout<<"successeful!\n";

//            auto read_buffer=std::make_shared<boost::asio::streambuf>();
//            std::ostream os(read_buffer.get());


//            os<<"HTTP/1.1 200 OK\r\n";
//            os<<"Server:httpserver\r\n";
//            os<<"Content-type:text-html\r\n";

//            os<<"\r\n";//空行
//            os<<"<html><body bgcolor=white>this is the html.<hr><p>hello word! web! </p><br></body></html>";

//            auto data_ptr = boost::asio::buffer_cast<const char* >(read_buffer->data());


            std::string reply;
            reply+="HTTP/1.1 200 OK\r\n";
            reply+="Server:httpserver\r\n";
            reply+="Content-type:text-html\r\n";
            reply+="\r\n";//空行
            reply+="<html><body bgcolor=white>this is the html.<hr><p>hello word! web! </p><br></body></html>";

            memcpy(m_readbuf,reply.c_str(),reply.size());



            _socket->async_write_some(buffer(m_readbuf,reply.size()),boost::bind(&Http_server::handle_write,this,_socket,boost::asio::placeholders::error,boost::asio::placeholders::bytes_transferred));



        }




    }

    void handle_write(std::shared_ptr<tcp::socket> _socket,const boost::system::error_code& error,std::size_t){
        //to do ...
    }
    void run(){
        m_io_context.run();
    }

    void close()//关闭socket连接
    {
        error_code ec;
       //to do close connect
        m_socket->close();
    }

    void cancel_timeout() noexcept //取消超时
    {
        if(timer) {
            error_code ec;
            timer->cancel(ec);
        }
    }


    void set_timeout(long seconds) noexcept //设置超时时长
    {
        if(seconds == 0) {
            timer = nullptr;
            return;
        }

        timer = std::unique_ptr<boost::asio::steady_timer>(new boost::asio::steady_timer(m_io_context));
        timer->expires_from_now(std::chrono::seconds(seconds));
        auto self = this->shared_from_this();
        timer->async_wait([self](const error_code &ec) {
            if(!ec)
                self->close();//超时关闭连接
        });
    }



private:
    boost::asio::io_context& m_io_context;//io上下文对象
    tcp::acceptor m_acceptor;//接收器
    std::shared_ptr<tcp::socket> m_socket;
    std::unique_ptr<boost::asio::steady_timer> timer;  //定时器
    enum{MAX_length=1024};
    char m_readbuf[MAX_length];//接收到的数据

};

int main()
{


    try {
        boost::asio::io_context io;
        Http_server http_server(io);

        http_server.run();


    }  catch (std::exception &e) {
        std::cout<<e.what()<<"\n";
    }

    std::cout<<"back to main()\n";
    return 0;
}
