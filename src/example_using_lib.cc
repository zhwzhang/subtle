#include <iostream>

#include "rpc_impl.h"
#include "subtle.h"

int main(int argc, char** argv) {
  if (argc != 2) {
    std::cout << "Usage: ./subtle path/to/video.mvk" << std::endl;
    exit(1);
  }

  libsubtle::XmlRpcImpl client;
  libsubtle::Subtle s(&client);
  s.DownloadSubtitles("eng", argv[1]);
}
