cmake_minimum_required( VERSION 3.23 )

project( WebServer 
         VERSION 0.1.0
         DESCRIPTION "Description of project"
         HOMEPAGE_URL "http://www.abc.com"
         LANGUAGES CXX )
         
#execute_process(COMMAND g++ -std=c++20 -xc++-system-header -fmodules-ts
#         iostream
#         string
#)
         
add_executable( WebServer main.cpp)

target_sources( WebServer PRIVATE 
                main.cpp connection.h connection.cpp
                session.h session.cpp
                http/v2/hpack.cpp
                http/v2/hpack.h
                http/v2/encoder.cpp
                http/v2/encoder.h
                http/v2/decoder.cpp
                http/v2/decoder.h
                tls.h   tls.cpp
                http2server.cpp http2server.h
                http/v2/http2_stream.h http/v2/http2_stream.cpp
                http/v2/frame.h http/v2/frame.cpp
                http/httpresponse.h
                http/httpresponse.cpp
                http/httprequest.h
                http/httprequest.cpp
                log/access_log.h
                log/access_log.cpp
                log/logs.h
                log/logs.cpp
                log/log_queue.h
                log/log_queue.cpp
                log/error_log.h
                log/error_log.cpp
                log/log.h
                log/log.cpp)


                
target_compile_features( WebServer PRIVATE cxx_std_20 )
target_compile_definitions(WebServer
    PRIVATE
        BOOST_ASIO_HAS_IO_URING=1
        BOOST_ASIO_DISABLE_EPOLL=1
 )

#target_compile_options( WebServer PRIVATE -fmodules-ts )

#target_include_directories( WebServer PRIVATE /usr/include )

#target_link_directories( WebServer PRIVATE /usr/lib )

target_link_libraries( WebServer PRIVATE uring ssl crypto)

#set_target_properties( WebServer PROPERTIES
            #CXX_STANDARD 20
            #CXX_EXTENSIONS OFF
            #etc..
            #)
# 设置指定的C++编译器版本是必须的，如果不设置，或者为OFF，则指定版本不可用时，会使用上一版本。
#set_target_properties(WebServer PROPERTIES  CXX_STANDARD_REQUIRED on)


## --------------------------------------------------------------------------
## Adding build-requirements
## 源文件列表
#target_sources( <target> PRIVATE <source-file>... )
#
#target_precompile_headers( <target> PRIVATE <header-file>... )
#
## 预处理头文件搜索路径列表，等价参数：-I
#target_include_directories( <target> PRIVATE <include-search-dir>... )  
#
## 预编译宏 自动在指定的宏前添加，等价参数：-D
#target_compile_definitions( <target> PRIVATE <macro-definitions>... )  
#
## 编译选项和特性
#target_compile_options( <target> PRIVATE <compiler-option>... )
#target_compile_features( <target> PRIVATE <feature>... )
#
## 链接库
#target_link_libraries( <target> PRIVATE <dependency>... )
#
## 链接选项
#target_link_options( <target> PRIVATE <linker-option>... )
#
## 链接库搜索路径，等价参数：-L
#target_link_directories( <target> PRIVATE <linker-search-dir>... )

#execute_process(COMMAND ln -s /opt/gcc/gcm.cache  ./)

## Modern Cmake建议围绕Target和Property进行项目配置，竭力避免出现下面变量variable的set定义。
## Variable横行是典型CMake2.8时期的风格, Modern CMake则遵循了OO方法。其中，target表示待构建的对象，
## target相关的语句用于定义来约束link、compile等相关属性的scope。
#set(CMAKE_CXX_STANDARD 20)
#set(CMAKE_CXX_STANDARD_REQUIRED ON)

#SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
