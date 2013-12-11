#include <iostream>
#include <utility>

#include "gtest/gtest.h"
#include "src/rpc_impl.h"

using std::ios;
using std::ifstream;
using std::pair;
using std::make_pair;

namespace libsubtle {

const string kServerUrl = "http://api.opensubtitles.org/xml-rpc";
const string kUserAgent = "OS Test User Agent";

TEST(XmlRpc, LogIn) {
  XmlRpcImpl client;
  client.Init(kUserAgent, kServerUrl);

  // login
  LoginRequest* lr = new LoginRequest();
  string token = client.LogIn(lr).token_;
  delete lr;
  ASSERT_NE("", token);
}

TEST(XmlRpc, LogOut) {
  XmlRpcImpl client;
  client.Init(kUserAgent, kServerUrl);

  // login
  LoginRequest* lr = new LoginRequest();
  string token = client.LogIn(lr).token_;
  delete lr;
  ASSERT_EQ(OK, client.LogOut(token).GetStatus());
}

TEST(XmlRpc, NoOperation) {
  XmlRpcImpl client;
  client.Init(kUserAgent, kServerUrl);

  // login
  LoginRequest* lr = new LoginRequest();
  string token = client.LogIn(lr).token_;
  delete lr;
  ASSERT_EQ(OK, client.NoOperation(token).GetStatus());
}

TEST(XmlRpc, Search) {
  XmlRpcImpl client;
  client.Init(kUserAgent, kServerUrl);

  // login
  LoginRequest* lr = new LoginRequest();
  string token = client.LogIn(lr).token_;
  delete lr;

  vector<SubFile> result;
  SearchRequest* req = new SearchRequest("eng", "7d9cd5def91c9432", 735934464);
  SearchResponse res = client.SearchSubtitles(token, req);

  ASSERT_NE(0, res.data_.size());
  ASSERT_EQ(OK, res.GetStatus());
  res.data_[0].PrintTitle();
  res.data_[0].Print();
}

TEST(XmlRpc, SearchImdb) {
  XmlRpcImpl client;
  client.Init(kUserAgent, kServerUrl);

  // login
  LoginRequest* lr = new LoginRequest();
  string token = client.LogIn(lr).token_;
  delete lr;

  vector<SubFile> result;
  SearchRequest* req = new SearchRequest("hrv", "0068646");
  SearchResponse res = client.SearchSubtitles(token, req);

  ASSERT_NE(0, res.data_.size());
  ASSERT_EQ(OK, res.GetStatus());
}

TEST(XmlRpc, SearchEmpty) {
  XmlRpcImpl client;
  client.Init(kUserAgent, kServerUrl);

  // login
  LoginRequest* lr = new LoginRequest();
  string token = client.LogIn(lr).token_;
  delete lr;

  vector<SubFile> result;
  SearchRequest* req = new SearchRequest("wrong lang", "7d9cd5def91c9432",
      735934464);
  SearchResponse res = client.SearchSubtitles(token, req);

  ASSERT_EQ(0, res.data_.size());
  ASSERT_EQ(OK, res.GetStatus());
}

TEST(XmlRpc, SearchMailNoAuth) {
  XmlRpcImpl client;
  client.Init(kUserAgent, kServerUrl);

  // login
  LoginRequest* lr = new LoginRequest();
  string token = client.LogIn(lr).token_;
  delete lr;

  vector<string> languages;
  languages.push_back("eng");

  vector<pair<string, double> > movies;
  movies.push_back(make_pair("7d9cd5def91c9432", 735934464));

  SearchMailRequest* req = new SearchMailRequest(languages, movies);
  SearchMailResponse res = client.SearchMailSubtitles(token, req);

  ASSERT_EQ(UNAUTHORIZED, res.GetStatus());
}

TEST(XmlRpc, Download) {
  XmlRpcImpl client;
  client.Init(kUserAgent, kServerUrl);

  // login
  LoginRequest* lr = new LoginRequest();
  string token = client.LogIn(lr).token_;
  delete lr;

  vector<int> subtitles;
  subtitles.push_back(144635);
  DownloadRequest* req = new DownloadRequest(subtitles);
  DownloadResponse res = client.DownloadSubtitles(token, req);

  ASSERT_NE(0, res.subtitles_.size());
  ASSERT_EQ(OK, res.GetStatus());
}

TEST(XmlRpc, ServerInfo) {
  XmlRpcImpl client;
  client.Init(kUserAgent, kServerUrl);
  ServerInfoResponse res = client.ServerInfo();

  for (map<string, string>::const_iterator it = res.last_update_strings_.begin()
       ; it != res.last_update_strings_.end(); ++it) {
    std::cout << it->first << " " << it->second << std::endl;
  }
  std::cout << "xml rpc version " << res.xmlrpc_version_ << std::endl;
}

TEST(XmlRpc, ReportWrongMovieHash) {
  XmlRpcImpl client;
  client.Init(kUserAgent, kServerUrl);

  // login
  LoginRequest* lr = new LoginRequest();
  string token = client.LogIn(lr).token_;
  delete lr;

  ReportWrongMovieHashRequest* req = new ReportWrongMovieHashRequest(-1);
  ReportWrongMovieHashResponse res = client.ReportWrongMovieHash(token, req);
  delete req;

  ASSERT_EQ(OK, res.GetStatus());
}

TEST(XmlRpc, SubtitlesVoteAnonymous) {
  XmlRpcImpl client;
  client.Init(kUserAgent, kServerUrl);

  // login
  LoginRequest* lr = new LoginRequest();
  string token = client.LogIn(lr).token_;
  delete lr;

  SubtitlesVoteRequest* req = new SubtitlesVoteRequest(735934464, 5);
  SubtitlesVoteResponse res = client.SubtitlesVote(token, req);
  delete req;

  ASSERT_EQ(UNAUTHORIZED, res.GetStatus());
}

TEST(XmlRpc, SubtitlesVoteLoggedIn) {
  XmlRpcImpl client;
  client.Init(kUserAgent, kServerUrl);

  // login
  LoginRequest* lr = new LoginRequest("stgpetrovic", "john2222", "eng");
  string token = client.LogIn(lr).token_;
  delete lr;

  SubtitlesVoteRequest* req = new SubtitlesVoteRequest(735934464, 5);
  SubtitlesVoteResponse res = client.SubtitlesVote(token, req);
  delete req;

  ASSERT_EQ(OK, res.GetStatus());
}

TEST(XmlRpc, SubtitlesVoteMovieNotExist) {
  XmlRpcImpl client;
  client.Init(kUserAgent, kServerUrl);

  // login
  LoginRequest* lr = new LoginRequest("stgpetrovic", "john2222", "eng");
  string token = client.LogIn(lr).token_;
  delete lr;

  SubtitlesVoteRequest* req = new SubtitlesVoteRequest(0, 5);
  SubtitlesVoteResponse res = client.SubtitlesVote(token, req);
  delete req;

  ASSERT_EQ(INVALID_PARAMS, res.GetStatus());
}

TEST(XmlRpc, AddCommentAnonymous) {
  XmlRpcImpl client;
  client.Init(kUserAgent, kServerUrl);

  // login
  LoginRequest* lr = new LoginRequest();
  string token = client.LogIn(lr).token_;
  delete lr;

  AddCommentRequest* req = new AddCommentRequest(735934464, "test", true);
  AddCommentResponse res = client.AddComment(token, req);
  delete req;

  ASSERT_EQ(UNAUTHORIZED, res.GetStatus());
}

TEST(XmlRpc, AddCommentLoggedIn) {
  XmlRpcImpl client;
  client.Init(kUserAgent, kServerUrl);

  // login
  LoginRequest* lr = new LoginRequest("stgpetrovic", "john2222", "eng");
  string token = client.LogIn(lr).token_;
  delete lr;

  AddCommentRequest* req = new AddCommentRequest(735934464, "test", false);
  AddCommentResponse res = client.AddComment(token, req);
  delete req;

  ASSERT_EQ(OK, res.GetStatus());
}

TEST(XmlRpc, CheckMovieHash) {
  XmlRpcImpl client;
  client.Init(kUserAgent, kServerUrl);

  // login
  LoginRequest* lr = new LoginRequest("", "", "");
  string token = client.LogIn(lr).token_;
  delete lr;

  vector<string> hashes = {"7d9cd5def91c9432", "8e245d9679d31e12"};
  CheckMovieHashRequest* req = new CheckMovieHashRequest(hashes);
  CheckMovieHashResponse res = client.CheckMovieHash(token, req);
  delete req;

  for (const auto& movie_info : res.movie_infos_) {
    movie_info.second.Print();
  }

  ASSERT_EQ(OK, res.GetStatus());
}

TEST(XmlRpc, CheckSubHash) {
  XmlRpcImpl client;
  client.Init(kUserAgent, kServerUrl);

  // login
  LoginRequest* lr = new LoginRequest("", "", "");
  string token = client.LogIn(lr).token_;
  delete lr;

  vector<string> hashes = {"579aa0689606dd1ecbcaadc8d447b9a9", "wrong"};
  CheckSubHashRequest* req = new CheckSubHashRequest(hashes);
  CheckSubHashResponse res = client.CheckSubHash(token, req);
  delete req;

  for (const auto& sub_ids : res.sub_ids_) {
    std::cout << "Sub hash: " << sub_ids.first
              << ", Sub id: " << sub_ids.second << std::endl;
  }

  ASSERT_EQ(OK, res.GetStatus());
  ASSERT_EQ("0", res.sub_ids_["wrong"]);
  ASSERT_EQ("1953638724", res.sub_ids_["579aa0689606dd1ecbcaadc8d447b9a9"]);
}

TEST(XmlRpc, GetSubLanguages) {
  XmlRpcImpl client;
  client.Init(kUserAgent, kServerUrl);

  GetSubLanguagesRequest* req = new GetSubLanguagesRequest("en");

  GetSubLanguagesResponse res = client.GetSubLanguages(req);
  delete req;

  for (const auto& lang_info : res.lang_infos_) {
    lang_info.Print();
  }

  ASSERT_EQ(OK, res.GetStatus());
  ASSERT_GT(res.lang_infos_.size(), 10);
}

TEST(XmlRpc, DetectLanguage) {
  XmlRpcImpl client;
  client.Init(kUserAgent, kServerUrl);

  // login
  LoginRequest* lr = new LoginRequest("", "", "");
  string token = client.LogIn(lr).token_;
  delete lr;

  vector<string> texts = {"This is English", "Ovo je hrvatski jezik."};
  DetectLanguageRequest* req = new DetectLanguageRequest(texts);

  DetectLanguageResponse res = client.DetectLanguage(token, req);
  delete req;

  // I don't care if it guessed right, just that the request works.
  for (const auto& detected_langs : res.detected_langs_) {
    ASSERT_EQ(32, detected_langs.first.length());  // md5 is 16bit = 32 in hex
    ASSERT_EQ(3, detected_langs.second.length());  // language code is 3 letters
  }
  ASSERT_EQ(OK, res.GetStatus());
}

TEST(XmlRpc, GetAvailableTranslations) {
  XmlRpcImpl client;
  client.Init(kUserAgent, kServerUrl);

  // login
  LoginRequest* lr = new LoginRequest("", "", "");
  string token = client.LogIn(lr).token_;
  delete lr;

  GetAvailableTranslationsRequest* req = new GetAvailableTranslationsRequest(
      "oscar");

  GetAvailableTranslationsResponse res = client.GetAvailableTranslations(token,
                                                                         req);
  delete req;

  // I don't care what it returns, just that the request works.
  for (const auto& supported_langs : res.translation_infos_) {
    std::cout << supported_langs.first << std::endl;
    supported_langs.second.Print();
  }

  ASSERT_EQ(OK, res.GetStatus());
  ASSERT_GT(res.translation_infos_.size(), 0);
}

TEST(XmlRpc, GetTranslation) {
  XmlRpcImpl client;
  client.Init(kUserAgent, kServerUrl);

  // login
  LoginRequest* lr = new LoginRequest("", "", "");
  string token = client.LogIn(lr).token_;
  delete lr;

  // This does not work (server-side)...
  // GetTranslationRequest* req = new GetTranslationRequest("en", "po",
  //                                                        "oscar");

  // GetTranslationResponse res = client.GetTranslation(token, req);
  // delete req;

  // // I don't care what it returns, just that the request works.
  // std::cout << res.translation_;

  // ASSERT_EQ(OK, res.GetStatus());
  // ASSERT_GT(res.translation_.length(), 0);
}

TEST(XmlRpc, GetTranslationWrongParams) {
  XmlRpcImpl client;
  client.Init(kUserAgent, kServerUrl);

  // login
  LoginRequest* lr = new LoginRequest("", "", "");
  string token = client.LogIn(lr).token_;
  delete lr;

  GetTranslationRequest* req = new GetTranslationRequest("en", "po", "oscar");

  bool threw = false;
  try {
    GetTranslationResponse res = client.GetTranslation(token, req);
  } catch(SubtleException e) {
    threw = true;
    std::cout << e.what() << std::endl;
  }

  delete req;
  if (!threw) FAIL();
}

TEST(XmlRpc, AutoUpdate) {
  XmlRpcImpl client;
  client.Init(kUserAgent, kServerUrl);

  AutoUpdateRequest* req = new AutoUpdateRequest("oscar");

  AutoUpdateResponse res = client.AutoUpdate(req);
  delete req;

  // I don't care what it returns, just that the request works.
  ASSERT_EQ(OK, res.GetStatus());
  ASSERT_GT(res.version_.length(), 0);
  ASSERT_GT(res.comments_.length(), 0);
  ASSERT_GT(res.url_windows_.length() + res.url_linux_.length(), 0);
}

TEST(XmlRpc, SearchMoviesOnImdb) {
  XmlRpcImpl client;
  client.Init(kUserAgent, kServerUrl);

  // login
  LoginRequest* lr = new LoginRequest("", "", "");
  string token = client.LogIn(lr).token_;
  delete lr;

  SearchMoviesOnImdbRequest* req = new SearchMoviesOnImdbRequest("Godfather");

  SearchMoviesOnImdbResponse res = client.SearchMoviesOnImdb(token, req);
  delete req;

  for (const auto& result : res.imdb_results_) {
    std::cout << result.id << ": " << result.title << std::endl;
  }

  ASSERT_EQ(OK, res.GetStatus());
  ASSERT_GT(res.imdb_results_.size(), 0);
}

TEST(XmlRpc, GetImdbMovieDetails) {
  XmlRpcImpl client;
  client.Init(kUserAgent, kServerUrl);

  // login
  LoginRequest* lr = new LoginRequest("", "", "");
  string token = client.LogIn(lr).token_;
  delete lr;

  GetImdbMovieDetailsRequest* req = new GetImdbMovieDetailsRequest("0092007");

  GetImdbMovieDetailsResponse res = client.GetImdbMovieDetails(token, req);
  delete req;

  ASSERT_EQ(OK, res.GetStatus());
}

TEST(XmlRpc, InsertMovie) {
  XmlRpcImpl client;
  client.Init(kUserAgent, kServerUrl);

  // login
  LoginRequest* lr = new LoginRequest("stgpetrovic", "john2222", "eng");
  string token = client.LogIn(lr).token_;
  delete lr;

  InsertMovieRequest* req = new InsertMovieRequest(
    "Three koalas and the tiger Volume 1",
    "2008");

  InsertMovieResponse res = client.InsertMovie(token, req);
  delete req;
  std::cout << "movie added as: " << res.id_ << std::endl;

  ASSERT_EQ(OK, res.GetStatus());
  ASSERT_GT(res.id_.length(), 0);
}

}  // namespace libsubtle
