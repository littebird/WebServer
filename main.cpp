//a webserver by c++ implement
#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/asio/steady_timer.hpp>
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
        m_acceptor.async_accept(*m_socket,boost::bind(&Http_server::handle_accept,this,m_socket,boost::asio::placeholders::error/*占位符*/));  //异步等待连接

    }

    void handle_accept(std::shared_ptr<tcp::socket> _socket,const boost::system::error_code& error){


        //after accept do something
        if(error){//错误码检查
            std::cout<<"error!\n";
            return;
        }

        _socket->async_read_some(buffer(m_readbuf,MAX_length),boost::bind(&Http_server::handle_read,this,_socket,boost::asio::placeholders::error));


    }

    void handle_read(std::shared_ptr<tcp::socket> _socket,const boost::system::error_code& error){
        if(!error){
            std::cout<<m_readbuf<<std::endl;
            std::cout<<"successeful!\n";

            std::string reply="HTTP/1.1 200 OK\r\n";
            reply +="Server:httpserver\r\n";
            reply +="Content-type:text-html\r\n";

            reply +="\r\n";//空行
            reply +=" hello cc";
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

    std::string ParseMsgContent(std::string msg)//解析请求报文，去掉报文头，保留报文消息体
    {
        std::string content;
        int length = msg.size();
        std::string::size_type pos;
        while ((pos = msg.rfind("\r")) != std::string::npos)
        {
            if (pos >= msg.size()-2)
            {
                msg = msg.substr(0, pos);
                continue;
            }
            else
            {
                content = msg.substr(pos + 1, msg.size() - pos);
                break;
            }
        }
        return content;
    }

    void close()//关闭socket连接
    {
        error_code ec;
       //to do close connect
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
