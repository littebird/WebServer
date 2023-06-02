# WebServer
use c++11 create a webserver
based on Asio
## 1.使用技术
* C++11
* 基于Asio网络库，异步io事件处理线程池
* 异步请求处理，异步响应处理
* 封装http协议(http1.1)，长连接传输，%编解码，query_string，解析get,post请求处理静态资源
* 使用Aiso网络库steady_timer定时器实现长连接及超时请求
* http请求解析状态机，使用正则表达式regex库解析请求
* 异步日志系统，单独创建日志线程处理，记录错误及访问日志，多态实现
* 采用日志队列logQueue实现日志的获取，从而将服务器运行状态记录在文件中
* 增加http2协议（to do）hpack头部压缩，多路复用，二进制分帧。
