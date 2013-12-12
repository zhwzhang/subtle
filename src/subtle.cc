#include <inttypes.h>

#include <cstdio>
#include <cstring>

#include <fstream>

#include "src/base64.h"
#include "src/subtle.h"
#include "src/types.h"

using std::cout;
using std::endl;
using std::map;
using std::ofstream;
using std::pair;
using std::string;
using std::vector;

namespace libsubtle {

const string Subtle::kServerUrl = "http://api.opensubtitles.org/xml-rpc";
const string Subtle::kUserAgent = "libsubtle";

Subtle::~Subtle() {
  if (!token_.empty()) {
    client_->LogOut(token_);
  }
}

extern "C" Subtle::Subtle(XmlRpcClient* client) : client_(client) {
  LoginRequest* req = new LoginRequest();
  client_->Init(kUserAgent, kServerUrl);
  LoginResponse res = client_->LogIn(req);
  token_ = res.token_;
  delete req;
}

extern "C" vector<SubFile> Subtle::SearchSubtitles(string lang, string hash,
    double size) {
  SearchRequest* req = new SearchRequest(lang, hash, size);
  SearchResponse res = client_->SearchSubtitles(token_, req);

  delete req;

  return res.data_;
}

extern "C" void Subtle::DownloadSubtitles(string lang, string hash,
    double size) {
  auto search = SearchSubtitles(lang, hash, size);
  DownloadResponse res;

  if (!search.empty()) {
    vector<int> ids;
    int best_match = std::stoi(search[0].IDSubtitleFile_);
    ids.push_back(best_match);
    DownloadRequest* req = new DownloadRequest(ids);
    res = client_->DownloadSubtitles(token_, req);

    if (!res.subtitles_.empty()) {
      char *tmpname = strdup("/tmp/tmpfileXXXXXX.gz");
      mkstemp(tmpname);
      ofstream f(tmpname, ios::out | ios::binary);
      f << base64_decode(res.subtitles_[0].second);
      system(("gunzip -c " + std::string(tmpname) + " > " +
             search[0].SubFileName_).c_str());
      cout << "Downloaded subtitle to " << search[0].SubFileName_ << endl;
    }

    delete req;
  }
}

}  // namespace libsubtle