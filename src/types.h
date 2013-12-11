#ifndef SRC_TYPES_H_
#define SRC_TYPES_H_

#include <exception>
#include <iostream>
#include <map>
#include <string>
#include <utility>
#include <vector>

#include "src/subfile.h"

using std::string;
using std::vector;
using std::pair;
using std::map;

namespace libsubtle {

struct SubtleException : public std::exception {
  std::string msg_;
  explicit SubtleException(std::string msg) : msg_(msg) {}
  ~SubtleException() throw() {}
  const char* what() const throw() { return msg_.c_str(); }
};

enum Status  {
  OK                = 200,  // OK
  PARTIAL           = 206,  // Partial content; message
  UNAUTHORIZED      = 401,  // Unauthorized
  INVALID_FORMAT    = 402,  // Subtitles has invalid format
  HASH_DIFFERENT    = 403,  // SubHashes (content and sent subhash) are not same
  INVALID_LANGUAGE  = 404,  // Subtitles has invalid language!
  MISSING_PARAMS    = 405,  // Not all mandatory parameters was specified
  NO_SESSION        = 406,  // No session
  DOWNLOAD_LIMIT    = 407,  // Download limit reached
  INVALID_PARAMS    = 408,  // Invalid parameters
  METHOD_NOT_FOUND  = 409,  // Method not found
  UNKNOWN_ERROR     = 410,  // Other or unknown error
  INVALID_USERAGENT = 411,  // Empty or invalid useragent
  FORMAT_FORMAT     = 412,  // %s has invalid format (reason)
  TEMPORARY_DOWN    = 501,  // Temporary down
  UNAVAILABLE       = 503,  // Service Unavailable
  MOVED             = 301   // Moved (to http://api.opensubtitles.org/xml-rpc)
};

typedef struct _MovieInfo {
  string MovieHash;
  string MovieImdbID;
  string MovieName;
  string MovieYear;

  _MovieInfo(string movie_hash, string movie_imdb_id, string movie_name,
             string movie_year) : MovieHash(movie_hash),
                                  MovieImdbID(movie_imdb_id),
                                  MovieName(movie_name),
                                  MovieYear(movie_year) {}
  void Print() const {
    std::cout << "MovieHash: " << MovieHash << std::endl
              << "MovieImdbId: " << MovieImdbID << std::endl
              << "MovieName: " << MovieName << std::endl
              << "MovieYear: " << MovieYear << std::endl;
  }
} MovieInfo;

typedef struct _LangInfo {
  string SubLanguageID;
  string LanguageName;
  string ISO639;

  _LangInfo(string sub_language_id, string language_name, string iso639)
      : SubLanguageID(sub_language_id),
        LanguageName(language_name),
        ISO639(iso639) {}
  void Print() const {
    std::cout << "SubLanguageID: " << SubLanguageID << std::endl
              << "LanguageName: " << LanguageName << std::endl
              << "ISO639: " << ISO639 << std::endl;
  }
} LangInfo;

typedef struct _TranslationInfo {
  string LastCreated;
  string StringsNo;

  _TranslationInfo(string last_created, string strings_no)
        : LastCreated(last_created),
          StringsNo(strings_no) {}
  void Print() const {
    std::cout << "LastCreated: " << LastCreated << std::endl
              << "StringsNo: " << StringsNo << std::endl;
  }
} TranslationInfo;

struct Result {
  Status status;
  string message;
};

struct ImdbEntry {
  string id;
  string title;
};

class Response {
 public:
  Status GetStatus() {return result_.status;}
  string StatusMessage() {return result_.message;}
  double Duration() {return seconds_;}
  void SetStatus(const string& message, double seconds) {
    result_.message = message;
    result_.status = (Status)std::stoi(message);
    seconds_ = seconds;
  }

 private:
  Result result_;
  double seconds_;
};

class SearchRequest {
 public:
  string sub_language_id_;
  string movie_hash_;
  double movie_byte_size_;
  string imdb_id_;

  SearchRequest(string sub_language_id, string movie_hash,
      double movie_byte_size)
    : sub_language_id_(sub_language_id),
      movie_hash_(movie_hash),
      movie_byte_size_(movie_byte_size) {}

  SearchRequest(string sub_language_id, string imdb_id)
    : sub_language_id_(sub_language_id),
      imdb_id_(imdb_id) {}
};

class SearchResponse : public Response {
 public:
  vector<SubFile> data_;
};

class SearchMailRequest {
 public:
  vector<string> languages_;
  vector<pair<string, double> > movies_;

  SearchMailRequest(vector<string> languages,
      vector<pair<string, double> > movies)
    : languages_(languages),
      movies_(movies) {}
};

class SearchMailResponse : public Response {
 public:
};

class DownloadRequest {
 public:
  vector<int> movies_;

  explicit DownloadRequest(vector<int> movies) : movies_(movies) {}
};

class DownloadResponse : public Response {
 public:
  vector<pair<string, string> > subtitles_;
};

class LoginRequest {
 public:
  string username_;
  string password_;
  string lang_;
  LoginRequest() {}
  LoginRequest(const string& username, const string& password,
      const string& lang) : username_(username),
                            password_(password),
                            lang_(lang) {}
};

class LoginResponse : public Response {
 public:
  string token_;
};

class LogOutResponse : public Response {};

class NoOperationResponse : public Response {};

class ServerInfoResponse : public Response {
 public:
  string xmlrpc_version_;
  string xmlrpc_url_;
  string application_;
  string contact_;
  string website_url_;
  string users_online_total_;
  string users_online_program_;
  string users_loggedin_;
  string users_max_alltime_;
  string users_registered_;
  string subs_downloads_;
  string subs_subtitle_files_;
  string movies_total_;
  string movies_aka_;
  string total_subtitles_languages_;

  map<string, string> last_update_strings_;
};

class ReportWrongMovieHashRequest {
 public:
  int64_t id_sub_movie_file_;

  explicit ReportWrongMovieHashRequest(int64_t id_sub_movie_file)
    : id_sub_movie_file_(id_sub_movie_file) {}
};

class ReportWrongMovieHashResponse : public Response {};

class SubtitlesVoteRequest {
 public:
  int64_t id_subtitle_;
  int16_t score_;
  SubtitlesVoteRequest(int64_t id_subtitle, int16_t score)
    : id_subtitle_(id_subtitle),
      score_(score) {}
};

class SubtitlesVoteResponse : public Response {
 public:
  string sub_ratings_;
  string sub_sum_votes_;
  string id_subtitle_;
};

class AddCommentRequest {
 public:
  int64_t id_subtitle_;
  string comment_;
  bool bad_subtitle_;
  AddCommentRequest(int64_t id_subtitle, const string& comment,
                    bool bad_subtitle)
    : id_subtitle_(id_subtitle),
      comment_(comment),
      bad_subtitle_(bad_subtitle) {}
};

class AddCommentResponse : public Response {};

class CheckMovieHashRequest {
 public:
  vector<string> movie_hashes_;

  explicit CheckMovieHashRequest(vector<string> movie_hashes)
    : movie_hashes_(movie_hashes) {}

  explicit CheckMovieHashRequest(const string& movie_hash) {
    movie_hashes_.push_back(movie_hash);
  }
};

class CheckMovieHashResponse : public Response {
 public:
  map<string, MovieInfo> movie_infos_;
};

class CheckSubHashRequest {
 public:
  vector<string> sub_hashes_;

  explicit CheckSubHashRequest(vector<string> sub_hashes)
    : sub_hashes_(sub_hashes) {}

  explicit CheckSubHashRequest(string sub_hash) {
    sub_hashes_.push_back(sub_hash);
  }
};

class CheckSubHashResponse : public Response {
 public:
  map<string, string> sub_ids_;
};

class GetSubLanguagesRequest {
 public:
  string lang_;

  GetSubLanguagesRequest() : lang_("en") {}
  explicit GetSubLanguagesRequest(string lang) : lang_(lang) {}
};

class GetSubLanguagesResponse : public Response {
 public:
  vector<LangInfo> lang_infos_;
};

class DetectLanguageRequest {
 public:
  vector<string> texts_;

  explicit DetectLanguageRequest(vector<string> texts) : texts_(texts) {}
};

class DetectLanguageResponse : public Response {
 public:
  map<string, string> detected_langs_;
};

class GetAvailableTranslationsRequest {
 public:
  string program_;

  explicit GetAvailableTranslationsRequest(const string& program)
    : program_(program) {}
};

class GetAvailableTranslationsResponse : public Response {
 public:
  map<string, TranslationInfo> translation_infos_;
};

class GetTranslationRequest {
 public:
  string iso639_;
  string format_;
  string program_;

  GetTranslationRequest(const string& iso639, const string& format,
                        const string& program)
                    : iso639_(iso639),
                      format_(format),
                      program_(program) {}
};

class GetTranslationResponse : public Response {
 public:
  string translation_;
};

class AutoUpdateRequest {
 public:
  string program_;

  explicit AutoUpdateRequest(string program) : program_(program) {}
};

class AutoUpdateResponse : public Response {
 public:
  string version_;
  string url_windows_;
  string url_linux_;
  string comments_;
};

class SearchMoviesOnImdbRequest {
 public:
  string query_;
  explicit SearchMoviesOnImdbRequest(string query) : query_(query) {}
};

class SearchMoviesOnImdbResponse : public Response {
 public:
  vector<ImdbEntry> imdb_results_;
};

class GetImdbMovieDetailsRequest {
 public:
  string imdb_id_;
  explicit GetImdbMovieDetailsRequest(string imdb_id) : imdb_id_(imdb_id) {}
};

class GetImdbMovieDetailsResponse : public Response {
 public:
  string id_;
  string title_;
  string year_;
  string cover_;

  map<string, string> cast_;
  map<string, string> directors_;
  map<string, string> writers_;

  string awards_;

  vector<string> genres_;
  vector<string> countries_;
  vector<string> languages_;

  string duration_;

  vector<string> certifications_;

  string tagline_;
  string plot_;
  string goofs_;
  string trivia_;
  string request_from_;

void Print() {
  std::cout << "id:" << id_<< std::endl <<
               "title:" << title_<< std::endl <<
               "year:" << year_<< std::endl <<
               "cover:" << cover_<< std::endl <<
               "awards:" << awards_<< std::endl <<
               "duration:" << duration_<< std::endl <<
               "tagline:" << tagline_<< std::endl <<
               "plot:" << plot_<< std::endl <<
               "goofs:" << goofs_<< std::endl <<
               "trivia:" << trivia_<< std::endl <<
               "request_from:" << request_from_<< std::endl;

  std::cout << "cast: ";
  for (const auto& actor : cast_) {
    std::cout << actor.second << "(" << actor.first << "), ";
  }
  std::cout << std::endl;

  std::cout << "writers: ";
  for (const auto& writer : writers_) {
    std::cout << writer.second << "(" << writer.first << "), ";
  }
  std::cout << std::endl;

  std::cout << "directors: ";
  for (const auto& director : directors_) {
    std::cout << director.second << "(" << director.first << "), ";
  }
  std::cout << std::endl;

  std::cout << "genres: ";
  for (const auto& genre : genres_) {
    std::cout << genre << ", ";
  }
  std::cout << std::endl;

  std::cout << "countries: ";
  for (const auto& country : countries_) {
    std::cout << country << ", ";
  }
  std::cout << std::endl;

  std::cout << "languages: ";
  for (const auto& language : languages_) {
    std::cout << language << ", ";
  }
  std::cout << std::endl;
}
};

class InsertMovieRequest {
 public:
  string movie_name_;
  string movie_year_;

  InsertMovieRequest(string movie_name, string movie_year)
      : movie_name_(movie_name),
        movie_year_(movie_year) {}
};

class InsertMovieResponse : public Response {
 public:
  string id_;
};

}  // namespace libsubtle

#endif  // SRC_TYPES_H_
