#include <iostream>
#include "boost/filesystem.hpp"
#include "boost/regex.hpp"

#include "src/rpc_impl.h"
#include "src/subtle.h"

using namespace boost::filesystem;

int main(int argc, char** argv) {
  libsubtle::XmlRpcImpl client;
  libsubtle::Subtle s(&client);

  path current_dir(".");
  boost::regex pattern(".*mkv|.*avi|.*mp4");
  for (recursive_directory_iterator iter(current_dir), end;
      iter != end;
      ++iter) {
    std::string name = iter->path().string();
    std::string dest = absolute(iter->path().parent_path()).string();
    if (regex_match(name, pattern))
      s.DownloadSubtitles(argv[1], name, dest);
  }
}
