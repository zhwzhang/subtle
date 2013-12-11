#ifndef SRC_RPC_IMPL_H_
#define SRC_RPC_IMPL_H_

#include <xmlrpc-c/base.hpp>
#include <xmlrpc-c/client_simple.hpp>

#include <string>

#include "src/xml_rpc_client.h"

using std::string;

namespace libsubtle {

class XmlRpcImpl : public XmlRpcClient {
 public:
  ~XmlRpcImpl();

  // Session handling
  LoginResponse LogIn(LoginRequest* req);
  LogOutResponse LogOut(const string& token);
  NoOperationResponse NoOperation(const string& token);

  // Search and download
  SearchResponse SearchSubtitles(const string& token, SearchRequest* req);
  SearchMailResponse SearchMailSubtitles(const string& token,
                                         SearchMailRequest* req);
  DownloadResponse DownloadSubtitles(const string& token, DownloadRequest* req);

  // Reporting and rating
  ServerInfoResponse ServerInfo();
  ReportWrongMovieHashResponse ReportWrongMovieHash(
    const string& token, ReportWrongMovieHashRequest* req);
  SubtitlesVoteResponse SubtitlesVote(const string& token,
                                      SubtitlesVoteRequest* req);
  AddCommentResponse AddComment(const string& token,
                                AddCommentRequest* req);


  // User interface
  GetSubLanguagesResponse GetSubLanguages(GetSubLanguagesRequest* req);
  DetectLanguageResponse DetectLanguage(const string& token,
                                        DetectLanguageRequest* req);
  GetAvailableTranslationsResponse GetAvailableTranslations(const string& token,
        GetAvailableTranslationsRequest* req);
  GetTranslationResponse GetTranslation(const string& token,
                                        GetTranslationRequest* req);
  AutoUpdateResponse AutoUpdate(AutoUpdateRequest* req);

  // Checking
  CheckMovieHashResponse CheckMovieHash(const string& token,
                                        CheckMovieHashRequest* req);
  CheckSubHashResponse CheckSubHash(const string& token,
                                    CheckSubHashRequest* req);

  // Movies
  SearchMoviesOnImdbResponse SearchMoviesOnImdb(const string& token,
                                                SearchMoviesOnImdbRequest* req);
  GetImdbMovieDetailsResponse GetImdbMovieDetails(const string& token,
        GetImdbMovieDetailsRequest* req);
  InsertMovieResponse InsertMovie(const string& token, InsertMovieRequest* req);

 private:
  xmlrpc_c::clientSimple client_;
};

}  // namespace libsubtle

#endif  // SRC_RPC_IMPL_H_
