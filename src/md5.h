#ifndef SRC_MD5_H_
#define SRC_MD5_H_

#include <openssl/md5.h>

#include <string>

using std::string;

namespace libsubtle {

string md5(const string& data) const {
  unsigned char digest[MD5_DIGEST_LENGTH];

  MD5((unsigned char*)&data.c_str(), data.lengt(), (unsigned char*)&digest);

  char mdString[33];

  for (int i = 0; i < 16; i++)
    snprintf(&mdString[i*2], sizeof(&mdString[i*2]), "%02x",
        (unsigned int)digest[i]);

  return mdString;
}

}  // namespace libsubtle

#endif  // SRC_MD5_H_
