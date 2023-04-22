//a webserver by c++ implement
#include <iostream>
#include "session.h"
#include"logs.h"
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

    std::cout<<"back to main()\n";
    return 0;
}
