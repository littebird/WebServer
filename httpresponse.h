#ifndef HTTPRESPONSE_H
#define HTTPRESPONSE_H
#include<iostream>
#include<vector>
#include<map>

//http响应类
class HttpResponse
{
public:
    HttpResponse();
    ~HttpResponse();

    std::string status;     //状态行
    std::vector<std::string> response_header; //响应头
    std::string blank;     //空行
    std::string response_body;               //响应体

    int status_code;        //状态码
    int fd;             //响应文件的fd


    std::map<int,std::string> CODE_STATUS;


};

#endif // HTTPRESPONSE_H
