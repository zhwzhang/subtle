#include <iostream>

#include "src/subfile.h"

using std::map;
using std::string;

namespace libsubtle {

SubFile::SubFile(map<string, string> data) {
  IDSubMovieFile_ = data["IDSubMovieFile"];
  MovieHash_ = data["MovieHash"];
  MovieByteSize_ = data["MovieByteSize"];
  MovieTimeMS_ = data["MovieTimeMS"];
  IDSubtitleFile_ = data["IDSubtitleFile"];
  SubFileName_ = data["SubFileName"];
  SubActualCD_ = data["SubActualCD"];
  SubSize_ = data["SubSize"];
  SubHash_ = data["SubHash"];
  IDSubtitle_ = data["IDSubtitle"];
  UserID_ = data["UserID"];
  SubLanguageID_ = data["SubLanguageID"];
  SubFormat_ = data["SubFormat"];
  SubSumCD_ = data["SubSumCD"];
  SubAuthorComment_ = data["SubAuthorComment"];
  SubAddDate_ = data["SubAddDate"];
  SubBad_ = data["SubBad"];
  SubRating_ = data["SubRating"];
  SubDownloadsCnt_ = data["SubDownloadsCnt"];
  MovieReleaseName_ = data["MovieReleaseName"];
  IDMovie_ = data["IDMovie"];
  IDMovieImdb_ = data["IDMovieImdb"];
  MovieName_ = data["MovieName"];
  MovieNameEng_ = data["MovieNameEng"];
  MovieYear_ = data["MovieYear"];
  MovieImdbRating_ = data["MovieImdbRating"];
  UserNickName_ = data["UserNickName"];
  ISO639_ = data["ISO639"];
  LanguageName_ = data["LanguageName"];
  SubDownloadLink_ = data["SubDownloadLink"];
  ZipDownloadLink_ = data["ZipDownloadLink"];
}

void SubFile::PrintTitle() {
  std::cout << MovieName_ << "(" << MovieYear_ << ") - www.imdb.com/title/tt"
    << IDMovieImdb_ << std::endl;
}

void SubFile::Print() {
  std::cout << "Language: " << SubLanguageID_ << std::endl
    << "Rating: " << SubRating_ << std::endl
    << "Downvotes: " << SubBad_ << std::endl
    << "Downloaded: " << SubDownloadsCnt_ << std::endl
    << "Uploaded by: " << UserNickName_ << std::endl
    << "Subtitle filename: " << SubFileName_ << std::endl
    << "Download URL: " << SubDownloadLink_ << std::endl;
}

}  // namespace libsubtle
