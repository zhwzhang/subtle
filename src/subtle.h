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

  virtual vector<SubFile> SearchSubtitles(const string& lng, const string& hash,
                                          double size) const;
  virtual void DownloadSubtitles(const string& lng, const string& hash,
                                 double size, const string& dest) const;
  virtual void DownloadSubtitles(const string& lng,
                                 const string& file_path,
                                 const string& dest) const;

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
