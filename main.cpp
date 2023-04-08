//a webserver by c++ implement
#include <iostream>
#include "session.h"
#include "httpresponse.h"
int main()
{
//        HttpResponse re("../WebServer/resource","/1.png",true);
//        re.buildResponse();

    try {
        const std::string& address="127.0.0.1";
        const std::string& port="8888";
        Session session(address,port);
        session.run();
    }  catch (std::exception &e) {
        std::cout<<e.what()<<"\n";
    }
    std::cout<<"back to main()\n";
    return 0;
}
