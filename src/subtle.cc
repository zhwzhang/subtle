#include <inttypes.h>
#include <sys/stat.h>

#include <cstdio>
#include <cstring>

#include <fstream>
#include <iostream>

#include "src/base64.h"
#include "src/subtle.h"
#include "src/types.h"
#include "src/gzstream.h"

const char kPathSeparator =
#ifdef _WIN32
    '\\';
#else
    '/';
#endif

using std::cout;
using std::endl;
using std::ifstream;
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

extern "C" vector<SubFile> Subtle::SearchSubtitles(const string& lng,
                                                   const string& hash,
                                                   double size) const {
  SearchRequest* req = new SearchRequest(lng, hash, size);
  SearchResponse res = client_->SearchSubtitles(token_, req);

  delete req;

  return res.data_;
}

extern "C" void Subtle::DownloadSubtitles(const string& lng, const string& hash,
                                          double size, const string& dest)
                                          const {
  auto search = SearchSubtitles(lng, hash, size);
  DownloadResponse res;

  if (!search.empty()) {
    vector<int> ids;
    int best_match = std::stoi(search[0].IDSubtitleFile_);
    ids.push_back(best_match);
    DownloadRequest* req = new DownloadRequest(ids);
    res = client_->DownloadSubtitles(token_, req);

    if (!res.subtitles_.empty()) {
      char temp_file[L_tmpnam];
      tmpnam(temp_file);
      ofstream f(temp_file, ios::out | ios::binary);
      f << base64_decode(res.subtitles_[0].second);
      igzstream in(temp_file);
      std::ofstream out(dest + kPathSeparator + search[0].SubFileName_);
      char c;
      while ( in.get(c)) {
        out << c;
      }
      in.close();
      out.close();
      cout << "Downloaded subtitle to " << search[0].SubFileName_ << endl;
      remove(temp_file);
    }

    delete req;
  }
}

extern "C" void Subtle::DownloadSubtitles(const string& lng,
                                          const string& file_path,
                                          const string& dest) const {
  ifstream f(file_path.c_str());
  if (!f.is_open()) {
    exit(1);
  }
  struct stat filestatus;
  stat(file_path.c_str(), &filestatus);
  Hasher hasher;
  DownloadSubtitles(lng, hasher.ComputeHashAsString(f),
                    static_cast<double>(filestatus.st_size),
                    dest);
}

}  // namespace libsubtle
