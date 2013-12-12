#ifndef SRC_SUBTLE_H_
#define SRC_SUBTLE_H_

#include <map>
#include <vector>
#include <cstdlib>
#include <string>

#include "src/hash.h"
#include "src/subfile.h"
#include "src/xml_rpc_client.h"

using std::string;
using std::vector;

#define FRIEND_TEST(test_case_name, test_name)\
  friend class test_case_name##_##test_name##_Test

namespace libsubtle {

class Subtle {
 public:
  explicit Subtle(XmlRpcClient* client);
  virtual ~Subtle();

  virtual vector<SubFile> SearchSubtitles(string lng, string hash, double size);
  virtual void DownloadSubtitles(string lng, string hash, double size);
  virtual void DownloadSubtitles(string lng, string file_path);

 private:
  FRIEND_TEST(Subtle, Login);
  bool Login();

  static const string kServerUrl;
  static const string kUserAgent;
  string token_;
  XmlRpcClient* client_;
};


}  // namespace libsubtle

#endif  // SRC_SUBTLE_H_
