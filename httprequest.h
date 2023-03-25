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

    //解析请求时，主状态机的状态
    enum PARSE_STATE {
            REQUEST_LINE,
            HEADERS,
            BODY,
            FINISH,
        };

    //解析行的状态，从状态机
    enum LINE_STATUS{
        LINE_OK=0,LINE_BAD,LINE_OPEN
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
    bool keepAlive()const;

    void init();

private:
    HTTP_CODE parse_request(std::string& text);      //主状态机,解析请求
    HTTP_CODE parse_request_line(const std::string& text); //解析请求首行
    HTTP_CODE parse_header(const std::string& text);       //解析请求头
    HTTP_CODE parse_body(const std::string& text);         //解析请求体

    LINE_STATUS parse_line();       //解析行-

    std::string m_request_line;   //请求行-
    std::vector<std::string> m_request_header;//请求头-
    std::string m_request_body;   //请求体

    //解析request请求
    std::string m_method;     //请求方法
    std::string m_uri;        //uri-
    std::string m_version;    //版本号
    std::map<std::string,std::string> header_kv;   //请求头中的键值对
    int m_content_length;     //请求体长度-
    std::string m_path;       //请求资源路径
    bool m_keepAlive;         //长连接

    PARSE_STATE m_parse_state;  //解析请求主状态机的状态



};

#endif // HTTPREQUEST_H
