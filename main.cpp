//a webserver by c++ implement
#include <iostream>
#include "session.h"
#include "log/logs.h"

#include "encoder.h"
#include "decoder.h"

int main()
{
    try {
        const std::string& address="127.0.0.1";
        const std::string& port="80";
        Logs *logs=Logs::get_instance();
        logs->init();
        Session session(address,port);
        session.run();
    }  catch (std::exception &e) {
        std::cout<<e.what()<<"\n";
    }

    return 0;
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

    std::string str;
    for(auto &ch:res){
//        std::cout<<(uint8_t)ch;
        str+=ch;
    }

    Hpack::DynamicTable responseTable;


    const uint8_t *ustrs=reinterpret_cast<const uint8_t*>(str.data());


    Decoder decode;

    if(!decode.decode(ustrs,str.length(),responseTable))
        std::cout<<"false"<<std::endl;

    std::cout<<responseTable[0].second<<"\n";
}
