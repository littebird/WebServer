#ifndef TLS_H
#define TLS_H

#include <memory>
#include <vector>
#include <boost/utility/string_ref.hpp>

namespace  tls{
const auto H2_ALPN=std::string("\x2h2");
const auto H2=std::string("h2");
bool check_h2_is_selected(const std::string& proto);

bool select_h2(const unsigned char **out,unsigned char*outlen,const unsigned char* in,unsigned char inlen);

bool select_proto(const unsigned char **out,unsigned char*outlen,const unsigned char* in,unsigned char inlen,const std::string& key);



}
#endif // TLS_H
