#include <chrono>
#include "httpresponse.h"

#define CRLF "\r\n"

HttpResponse::HttpResponse(const std::string& root,const std::string& path,bool keepalive):doc_root(root),m_path(path),keepAlive(keepalive),os(&buffer)
{
    m_status_code=CODE_STATUS::value::uninitialized;

    auto time=std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());//获得当前时间并to time_t
    std::string curTime=ctime(&time);
    _curTime=curTime.substr(0,curTime.size()-1);//去除末尾换行符

}

HttpResponse::HttpResponse():os(&buffer)
{

}

void HttpResponse::addStatusLine()
{
    os<<"HTTP/1.1 "<<m_status_code<<" "<<CODE_STATUS::get_string(m_status_code)<<CRLF;//响应状态行



}

void HttpResponse::addHeader()
{
    os<<"Server: webserver"<<CRLF;

    os<<"Date: "<<_curTime<<CRLF;
    if(keepAlive)
        os<<"Connection: keep-alive"<<CRLF;
    else os<<"Connection: close"<<CRLF;

    os<<"Content-type: "<<get_type(m_path)<<CRLF;

    if(!m_response_body.empty())
        os<<"Content-length: "<<m_response_body.size()<<CRLF;

    os<<CRLF;//空行

}



void HttpResponse::addBody()
{
    os<<m_response_body;

}
void HttpResponse::readFile()
{
    std::string full_path=doc_root+m_path;
    std::fstream ifs(full_path.c_str(),std::ios::in | std::ios::binary);//以读，二进制方式打开文件
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

void HttpResponse::buildH2Response(std::vector<std::pair<std::string,std::string>>& headers)
{

    m_status_code=CODE_STATUS::value::ok;
    readFile();
    headers.emplace_back("server","webserver");
    headers.emplace_back("date",curTime());
    headers.emplace_back("content-type",get_type(m_path));
    headers.emplace_back("content-length",std::to_string(m_response_body.size()));
    headers.emplace_back(":status",std::to_string(m_status_code));
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
        { ".wav",   "audio/wave"},
        { ".ogg",   "video/ogg"},
        { ".webm",  "video/webm"},
        { ".mp4",   "video/mpeg4"},
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







