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

HttpRequest::HTTP_CODE HttpRequest::parse_request(std::string &text)
{
    LINE_STATUS line_status=LINE_OK;
    HTTP_CODE  code=NO_REQUEST;



    return NO_REQUEST;
}


HttpRequest::HTTP_CODE HttpRequest::parse_request_line(const std::string& text)
{
    std::regex patten("^([^ ]*) ([^ ]*) HTTP/([^ ]*)$");
       std::smatch subMatch;
       if(regex_match(text, subMatch, patten)) {
           m_method = subMatch[1];
           m_path = subMatch[2];
           m_version = subMatch[3];
           m_parse_state = HEADERS;
           return GET_REQUEST;
       }
       return BAD_REQUEST;
}

HttpRequest::HTTP_CODE HttpRequest::parse_header(const std::string& text)
{
    std::regex patten("^([^:]*): ?(.*)$");
    std::smatch subMatch;
    if(regex_match(text, subMatch, patten)) {
        header_kv[subMatch[1]] = subMatch[2];
        return GET_REQUEST;
    }
    else {
        m_parse_state = BODY;
        return GET_REQUEST;
    }
    return BAD_REQUEST;
}

HttpRequest::HTTP_CODE HttpRequest::parse_body(const std::string& text)
{
    m_request_body=text;

    //if body not none method:post
    //to do parse body


    m_parse_state=FINISH;
    return NO_REQUEST;
}

HttpRequest::LINE_STATUS HttpRequest::parse_line()
{
    return LINE_OK;
}

