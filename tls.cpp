#include "tls.h"
#include "boost/asio/ssl.hpp"

namespace tls {


bool select_proto(const unsigned char **out, unsigned char *outlen, const unsigned char *in, unsigned char inlen, const std::string &key)
{
    for (auto p = in, end = in + inlen; p + key.size() <= end; p += *p + 1) {
      if (std::equal(std::begin(key), std::end(key), p)) {
        *out = p + 1;
        *outlen = *p;
        return true;
      }
    }
    return false;
}

bool select_h2(const unsigned char **out, unsigned char *outlen, const unsigned char *in, unsigned char inlen)
{
    return select_proto(out,outlen,in,inlen,H2_ALPN);
}

bool check_h2_is_selected(const std::string &proto)
{
    if(std::distance(H2.begin(),H2.end())!=std::distance(proto.begin(),proto.end()))
        return false;
    return std::equal(H2.begin(),H2.end(),proto.begin());
}


}
