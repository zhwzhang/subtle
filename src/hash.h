#ifndef SRC_HASH_H_
#define SRC_HASH_H_

#include <cinttypes>

#include <algorithm>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

typedef uint64_t MpcHash;
using std::ifstream;
using std::ios;

class Hasher {
 public:
  MpcHash ComputeHash(ifstream& f) const {
    MpcHash hash, fsize;

    f.seekg(0, ios::end);
    fsize = f.tellg();
    f.seekg(0, ios::beg);

    hash = fsize;
    for (MpcHash tmp = 0, i = 0; i < 65536/sizeof(tmp) && f.read(
          reinterpret_cast<char*>(&tmp), sizeof(tmp));
          i++, hash += tmp);
    f.seekg(Max(0, (MpcHash)fsize - 65536), ios::beg);
    for (MpcHash tmp = 0, i = 0; i < 65536/sizeof(tmp) && f.read(
          reinterpret_cast<char*>(&tmp), sizeof(tmp)); i++,
           hash += tmp);
    return hash;
  }

  std::string ComputeHashAsString(ifstream& f) const {
    std::stringstream s;
    s << std::setw(16) << std::setfill('0') << std::hex << ComputeHash(f);
    return s.str();
  }

 private:
  int Max(int x, int y) const {
    if ((x) > (y)) {
      return x;
    } else {
      return y;
    }
  }
};

#endif  // SRC_HASH_H_
