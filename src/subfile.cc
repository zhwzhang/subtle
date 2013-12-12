#include <iostream>

#include "src/subfile.h"

using std::map;
using std::string;

namespace libsubtle {

SubFile::SubFile(const map<string, string>& data) {
  IDSubMovieFile_ = data.at("IDSubMovieFile");
  MovieHash_ = data.at("MovieHash");
  MovieByteSize_ = data.at("MovieByteSize");
  MovieTimeMS_ = data.at("MovieTimeMS");
  IDSubtitleFile_ = data.at("IDSubtitleFile");
  SubFileName_ = data.at("SubFileName");
  SubActualCD_ = data.at("SubActualCD");
  SubSize_ = data.at("SubSize");
  SubHash_ = data.at("SubHash");
  IDSubtitle_ = data.at("IDSubtitle");
  UserID_ = data.at("UserID");
  SubLanguageID_ = data.at("SubLanguageID");
  SubFormat_ = data.at("SubFormat");
  SubSumCD_ = data.at("SubSumCD");
  SubAuthorComment_ = data.at("SubAuthorComment");
  SubAddDate_ = data.at("SubAddDate");
  SubBad_ = data.at("SubBad");
  SubRating_ = data.at("SubRating");
  SubDownloadsCnt_ = data.at("SubDownloadsCnt");
  MovieReleaseName_ = data.at("MovieReleaseName");
  IDMovie_ = data.at("IDMovie");
  IDMovieImdb_ = data.at("IDMovieImdb");
  MovieName_ = data.at("MovieName");
  MovieNameEng_ = data.at("MovieNameEng");
  MovieYear_ = data.at("MovieYear");
  MovieImdbRating_ = data.at("MovieImdbRating");
  UserNickName_ = data.at("UserNickName");
  ISO639_ = data.at("ISO639");
  LanguageName_ = data.at("LanguageName");
  SubDownloadLink_ = data.at("SubDownloadLink");
  ZipDownloadLink_ = data.at("ZipDownloadLink");
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
