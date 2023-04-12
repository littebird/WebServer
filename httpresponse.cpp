#include <chrono>
#include "httpresponse.h"

#define CRLF "\r\n"

HttpResponse::HttpResponse(const std::string& root,const std::string& path,bool keepalive):doc_root(root),m_path(path),keepAlive(keepalive),std::ostream(&buffer)
{
    m_status_code=CODE_STATUS::value::uninitialized;

}

HttpResponse::HttpResponse():std::ostream(&buffer)
{

}

void HttpResponse::addStatusLine()
{
    *this<<"HTTP/1.1 "<<m_status_code<<" "<<CODE_STATUS::get_string(m_status_code)<<CRLF;//响应状态行



}

void HttpResponse::addHeader()
{
    *this<<"Server: webserver"<<CRLF;

    *this<<"Date: "<<_curTime<<CRLF;
    if(keepAlive)
        *this<<"Connection: keep-alive"<<CRLF;
    else *this<<"Connection: close"<<CRLF;

    *this<<"Content-type: "<<get_type(m_path)<<CRLF;

    if(!m_response_body.empty())
        *this<<"Content-length: "<<m_response_body.size()<<CRLF;

    *this<<CRLF;//空行

}



void HttpResponse::addBody()
{
    *this<<m_response_body;

}
void HttpResponse::readFile()
{
    std::string full_path=doc_root+m_path;
    std::fstream ifs(full_path.c_str(),std::ios::in | std::ios::binary);//文件流
    if(!ifs){
        //error not found
        m_path="/404.html";
        m_status_code=CODE_STATUS::value::not_found;
        readFile();
    }
    char buf[512];
    while(ifs.read(buf,sizeof (buf)).gcount()>0){
        m_response_body.append(buf,ifs.gcount());
    }
}

void HttpResponse::buildResponse()
{

    m_status_code=CODE_STATUS::value::ok;
    readFile();
    addStatusLine();//添加状态行
    addHeader();//添加响应头
    addBody();//添加响应体

//    std::cout<<&buffer;

}



std::string HttpResponse::get_type(const std::string& path)
{
    const std::unordered_map<std::string,std::string> suffix_type{
        { ".html",  "text/html; charset=utf-8" },
        { ".xml",   "text/xml; charset=utf-8" },
        { ".xhtml", "application/xhtml+xml; charset=utf-8" },
        { ".txt",   "text/plain; charset=utf-8" },
        { ".pdf",   "application/pdf" },
        { ".png",   "image/png" },
        { ".gif",   "image/gif" },
        { ".jpg",   "image/jpeg" },
        { ".jpeg",  "image/jpeg" },
        { ".css",   "text/css "},
        { ".js",    "text/javascript "},
        { ".json",  "application/json; charset=utf-8"},
        { ".plain", "text/plain; charset=utf-8"}
    };
    auto findpos=path.find_last_of('.');//找到文件路径的后缀位置
    if(findpos!=std::string::npos){//如果存在
        std::string suffix=path.substr(findpos);//获取后缀

        auto it=suffix_type.find(suffix);
        if(it!=suffix_type.end()){
            return it->second;
        }
    }

    return "text/plain";//未找到,默认为text/plain类型
}







