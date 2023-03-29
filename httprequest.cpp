#include "httprequest.h"

HttpRequest::HttpRequest()
{

}

bool HttpRequest::keepAlive() const
{
    if(header_kv.count("Connection") == 1) {
        return header_kv.find("Connection")->second == "keep-alive" && m_version == "1.1";
    }
    return false;
}

void HttpRequest::init()
{
    m_parse_state=REQUEST_LINE;
}

HttpRequest::HTTP_CODE HttpRequest::parse_request(std::istream& readbuf)
{
    std::string line;

    while(getline(readbuf,line)&& m_parse_state != FINISH){
        line.pop_back();//去掉'/r'
        switch (m_parse_state)
        {
        case REQUEST_LINE:
            if(!parse_request_line(line));
                return BAD_REQUEST;

            break;
        case HEADERS:
            parse_header(line);
            m_keepAlive=keepAlive();
            break;

        default:
            break;

        }
    }

    return GET_REQUEST;
}


bool HttpRequest::parse_request_line(const std::string& text)
{
    std::regex patten("^([^ ]*) ([^ ]*) HTTP/([^ ]*)$");
       std::smatch subMatch;
       if(regex_match(text, subMatch, patten)) {
           m_method = subMatch[1];
           m_uri = subMatch[2];
           m_version = subMatch[3];
           m_parse_state = HEADERS;
           return true;
       }
       return false;
}

void HttpRequest::parse_header(const std::string& text)
{
    std::regex patten("^([^:]*): ?(.*)$");
    std::smatch subMatch;
    if(regex_match(text, subMatch, patten)) {
        header_kv[subMatch[1]] = subMatch[2];
    }
    else {
        m_parse_state = FINISH;
    }

}

void HttpRequest::parse_body(const std::string& text)
{
    //post request
    if(m_method=="POST"&&header_kv["content-Type"]=="application/x-www-form-urlencoded"){
        m_request_body=decode(text);

    }



}



bool HttpRequest::check_method()
{
    if(m_method=="POST" && header_kv.contains("Content-Length")){
        return true;
    }
    return false;
}

void HttpRequest::parse_uri()
{
    if(m_uri == "/") {
           m_uri = "/index.html";//初始页面
    }
}

std::string HttpRequest::decode(const std::string& text)
{
    std::string result;

    for(std::size_t i = 0; i < text.size(); ++i) {
        auto &chr = text[i];
        if(chr == '%' && i + 2 < text.size()) {
            auto hex = text.substr(i + 1, 2);
            auto decoded_chr = static_cast<char>(std::strtol(hex.c_str(), nullptr, 16));//从16进制转换为long并强制转换为char
            result += decoded_chr;
            i += 2;
        }
        else if(chr == '+')
            result += ' ';
        else
            result += chr;
    }
    return result;

}





