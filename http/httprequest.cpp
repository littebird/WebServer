#include "httprequest.h"

HttpRequest::HttpRequest()
{

}

bool HttpRequest::isKeepAlive()
{
    if(header_kv.count("Connection") == 1) {
        return header_kv.find("Connection")->second == "keep-alive" && m_version == "1.1";
    }
    return false;
}

void HttpRequest::init()
{
    m_parse_state=REQUEST_LINE;
    //初始化数据
}

HttpRequest::HTTP_CODE HttpRequest::parse_request(std::istream& is)
{
    std::string line;

    while(getline(is,line)&& m_parse_state != FINISH){
        line.pop_back();//去掉'/r'

        switch (m_parse_state)
        {
        case REQUEST_LINE:

            if(!parse_request_line(line))
                return BAD_REQUEST;
            parse_uri();
            break;
        case HEADERS:
            parse_header(line);
            m_keepAlive=isKeepAlive();
            break;
        case BODY:
            parse_body(line);
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
    else if(m_method=="POST"){
        m_parse_state = BODY;
    }else m_parse_state=FINISH;

}

void HttpRequest::parse_body(const std::string& text)
{
    //post request
    if(m_method=="POST"&&header_kv["Content-Type"]=="application/x-www-form-urlencoded"){//请求体body中使用了url编码
        m_request_body=decode(text);
    }else m_request_body=text;

    //to do store

    m_parse_state=FINISH;
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
           m_uri += "index.html";//默认页面
    }

    auto precentPos=m_uri.find('%');//uri中是否有%编码
    if(precentPos!=std::string::npos){
        m_uri=decode(m_uri);
    }


    if(m_method=="GET"){
        auto pos=m_uri.find('?');
        if(pos!=std::string::npos){//未到字符串尾，就找到？
            //uri中有querystring
            m_path=m_uri.substr(0,pos);
            m_query_string=m_uri.substr(pos+1);
            parse_query_string(m_query_string);

        }else //uri中不带参数
            m_path=m_uri;

    }else if (m_method=="POST") {
            m_path=m_uri;
    }



}


std::string HttpRequest::decode(const std::string& text)//%解码
{
    std::string result;

    for(std::size_t i = 0; i < text.size(); ++i) {
        auto& chr = text[i];
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

void HttpRequest::errorRes()
{
    m_path="/400.html";
}
void HttpRequest::parse_query_string(const std::string& text)
{
    std::size_t key_pos=0;
    auto key_end_pos=std::string::npos;
    auto value_pos=std::string::npos;

    for(std::size_t pos=0;pos<text.size();++pos){
        if(text[pos]=='='){
            key_end_pos=pos;//键结束的位置
            value_pos=pos+1;//值的位置
        }else if(text[pos]=='&'){//分隔标志符
            auto key=text.substr(key_pos,(key_end_pos-key_pos));//获得键
            if(!key.empty()){
                auto value=text.substr(value_pos,pos-value_pos);//获得值
                query_kv[key]=value;//放入query_kv
            }
            key_pos=pos+1;
            key_end_pos=std::string::npos;//重置key_end_pos和value_pos
            value_pos=std::string::npos;
        }
    }
    if(key_pos<text.size()){//最后一个键值
        auto key=text.substr(key_pos,key_end_pos-key_pos);
        if(!key.empty()){
            auto value=text.substr(value_pos);
            query_kv[key]=value;
        }
    }

}





