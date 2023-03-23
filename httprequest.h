#ifndef HTTPREQUEST_H
#define HTTPREQUEST_H

#include<iostream>
#include<vector>
#include <regex>
#include<map>

//http请求类
class HttpRequest
{
public:
    HttpRequest();
    ~HttpRequest();

    //解析请求的状态
    enum PARSE_STATE {
            REQUEST_LINE,
            HEADERS,
            BODY,
            FINISH,
        };


    enum HTTP_CODE {
           NO_REQUEST = 0,
           GET_REQUEST,
           BAD_REQUEST,
           NO_RESOURSE,
           FORBIDDENT_REQUEST,
           FILE_REQUEST,
           INTERNAL_ERROR,
           CLOSED_CONNECTION,
       };

private:
    std::string request_line;   //请求行
    std::vector<std::string> request_header;//请求头
    std::string request_body;   //请求体

    //解析request请求
    std::string method;     //请求方法
    std::string uri;        //uri
    std::string version;    //版本号
    std::map<std::string,std::string> header_kv;   //请求头中的键值对
    int content_length;     //请求体长度
    std::string path;       //请求资源路径
    bool keepAlive;         //长连接


};

#endif // HTTPREQUEST_H
