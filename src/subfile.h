#ifndef SRC_SUBFILE_H_
#define SRC_SUBFILE_H_

#include <map>
#include <string>

using std::map;
using std::string;

namespace libsubtle {

class SubFile {
 public:
  string IDSubMovieFile_;
  string MovieHash_;
  string MovieByteSize_;
  string MovieTimeMS_;
  string IDSubtitleFile_;
  string SubFileName_;
  string SubActualCD_;
  string SubSize_;
  string SubHash_;
  string IDSubtitle_;
  string UserID_;
  string SubLanguageID_;
  string SubFormat_;
  string SubSumCD_;
  string SubAuthorComment_;
  string SubAddDate_;
  string SubBad_;
  string SubRating_;
  string SubDownloadsCnt_;
  string MovieReleaseName_;
  string IDMovie_;
  string IDMovieImdb_;
  string MovieName_;
  string MovieNameEng_;
  string MovieYear_;
  string MovieImdbRating_;
  string UserNickName_;
  string ISO639_;
  string LanguageName_;
  string SubDownloadLink_;
  string ZipDownloadLink_;

  SubFile(map<string, string> data);

  void PrintTitle();
  void Print();
};

}  // namespace libsubtle

#endif  // SRC_SUBFILE_H_
