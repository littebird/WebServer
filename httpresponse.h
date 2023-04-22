#ifndef HTTPRESPONSE_H
#define HTTPRESPONSE_H
#include<iostream>
#include<vector>
#include<boost/asio/streambuf.hpp>
#include<fstream>
#include<unordered_map>
#include<chrono>


// HTTP Status codes
namespace CODE_STATUS {
// 状态码对应的值
enum value {
    uninitialized = 0,

    continue_code = 100,

    ok = 200,

    bad_request = 400,
    forbidden = 403,
    not_found = 404,
    method_not_allowed = 405,

    internal_server_error = 500,
    http_version_not_supported = 505,
};
inline std::string get_string(value code) {
    switch (code) {
    case uninitialized:
        return "Uninitialized";
    case continue_code:
        return "Continue";
    case ok:
        return "OK";
    case bad_request:
        return "Bad Request";
    case forbidden:
        return "Forbidden";
    case not_found:
        return "Not Found";
    case method_not_allowed:
        return "Method Not Allowed";
    case internal_server_error:
        return "Internal Server Error";
    case http_version_not_supported:
        return "HTTP Version Not Supported";
    default:
        return "Unknown";
    }
}
}


//http响应类
class HttpResponse
{
public:
    explicit HttpResponse(const std::string& root,const std::string& path,bool keepalive);
    HttpResponse();

    void addStatusLine();
    void addHeader();
    void addBody();
    void buildResponse();
    void readFile();
    std::string statusCode()
    {
        return CODE_STATUS::get_string(m_status_code);
    }
//    std::string m_status_line;     //状态行
//    std::vector<std::string> m_response_header; //响应头
    std::string m_response_body;               //响应体

    std::chrono::system_clock::time_point _curTime;    //当前响应时间
    std::string m_path;   //路径
    std::string doc_root;     //资源目录

    CODE_STATUS::value m_status_code;        //状态码
    bool keepAlive;         //长连接

    boost::asio::streambuf buffer;
    std::ostream os;

    std::string get_type(const std::string& suffix);//根据后缀获取资源类型



};


#endif // HTTPRESPONSE_H
