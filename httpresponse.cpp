#include "httpresponse.h"

HttpResponse::HttpResponse(const std::string& root):doc_root(root)
{
    buffer=std::make_shared<boost::asio::streambuf>();
}

void HttpResponse::addStatusLine()
{

}

void HttpResponse::addHeader()
{

}

void HttpResponse::addBody()
{

}

std::string HttpResponse::get_type(const std::string &suffix)
{
    const std::unordered_map<std::string,std::string> suffix_type{
        { ".html",  "text/html" },
        { ".xml",   "text/xml" },
        { ".xhtml", "application/xhtml+xml" },
        { ".txt",   "text/plain" },
        { ".pdf",   "application/pdf" },
        { ".png",   "image/png" },
        { ".gif",   "image/gif" },
        { ".jpg",   "image/jpeg" },
        { ".jpeg",  "image/jpeg" },
        { ".css",   "text/css "},
        { ".js",    "text/javascript "},
    };
    auto it=suffix_type.find(suffix);
    if(it!=suffix_type.end())
        return it->second;

    return "text/html";
}







