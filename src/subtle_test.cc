#include <iostream>

#include "gtest/gtest.h"
#include "src/subtle.h"
#include "src/rpc_impl.h"

using std::ios;
using std::ifstream;

namespace libsubtle {

TEST(Subtle, Search) {
  XmlRpcImpl client;
  Subtle s(&client);
  vector<SubFile> result;
  vector<SubFile> res = s.SearchSubtitles("eng", "7d9cd5def91c9432", 735934464);

  for (auto r : res) {
    r.Print();
  }
}

TEST(Subtle, Download) {
  XmlRpcImpl client;
  Subtle s(&client);
  vector<SubFile> result;
  s.DownloadSubtitles("eng", "7d9cd5def91c9432", 735934464);
}

TEST(Subtle, SearchFail) {
  XmlRpcImpl client;
  Subtle s(&client);
  vector<SubFile> result;
  vector<SubFile> res = s.SearchSubtitles("wrong lang", "7d9cd5def91c9432",
                                         735934464);
  ASSERT_NE(OK, res.size());
}

}  // namespace libsubtle
