#ifndef HTTPREQUEST_H
#define HTTPREQUEST_H

#include<iostream>
#include<vector>
#include<regex>
#include<unordered_map>
#include"../http2server.h"

//http请求类
class HttpRequest
{
    friend class Http2Server;
public:

    HttpRequest();


    //解析请求时，状态机的状态
    enum PARSE_STATE {
            REQUEST_LINE,
            HEADERS,
            BODY,
            FINISH,
        };
    //http请求解析结果
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

public:

    std::string uri()const{
        return m_uri;
    }
    std::string method()const{
        return m_method;
    }
    std::string version()const{
        return m_version;
    }
    std::string path()const{
        return m_path;
    }
    std::string request_body()const{
        return m_request_body;
    }
    bool keepAlive()const{
        return m_keepAlive;
    }
    bool  isKeepAlive();

    int body_size()
    {
        return m_request_body.size();
    }
    std::string userAgent()
    {
        std::string str="User-Agent";
        auto it=header_kv.find(str);
        return it->second;
    }
    void init();

    HTTP_CODE parse_request(std::istream& is);      //状态机,解析请求
    bool parse_request_line(const std::string& text); //解析请求首行
    void parse_header(const std::string& text);       //解析请求头
    void parse_body(const std::string& text);         //解析请求体
    bool check_method();    //检查请求方法
    void parse_uri();       //解析uri
    void parse_query_string(const std::string& text);   //解析查询字符串
    std::string decode(const std::string& text);       //百分号解码

    void errorRes();//错误请求

    std::unordered_map<std::string,std::string> header_kv;   //请求头中的键值对


private:
    std::string m_request_body; //请求体

    //解析request请求
    std::string m_method;     //请求方法
    std::string m_uri;        //uri
    std::string m_version;    //版本号
    std::string m_path;       //请求资源路径
    bool m_keepAlive;         //长连接
    std::string m_query_string; //uri中携带的参数
    std::unordered_map<std::string,std::string> query_kv;//存储解析参数后的数据


    PARSE_STATE m_parse_state;  //解析请求主状态机的状态




};

#endif // HTTPREQUEST_H
