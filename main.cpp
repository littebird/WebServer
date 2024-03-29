//a webserver by c++ implement
#include <iostream>
#include "session.h"
#include "log/logs.h"

int main()
{
//    void test_hpack();
    try {
        const std::string& address="127.0.0.1";
        const std::string& port="443";
        Logs *logs=Logs::get_instance();
        logs->init();
        Session session(address,port);
        session.run();
    }  catch (std::exception &e) {
        std::cout<<e.what()<<"\n";
    }

    return 0;

//    test_hpack();
}

void  test_hpack(){
    std::vector<std::pair<std::string,std::string>>headers;
    headers.emplace_back(":method","GET");
    headers.emplace_back(":scheme", "http");
    headers.emplace_back(":path", "/");
    headers.emplace_back(":authority", "www.example.com");

    Encoder encoder;
    Hpack::DynamicTable requestTable;

    std::vector<char> res;
    encoder.encode(res,headers,requestTable);

    std::string full_path{"/root/hpack"};
    std::ofstream ofs(full_path.c_str(),std::ios::out | std::ios::binary);//以写，二进制方式打开文件

    std::string str;
    for(auto &ch:res){
        ofs<<ch;
        str+=ch;
    }


}
