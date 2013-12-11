#ifndef SRC_XML_RPC_CLIENT_H_
#define SRC_XML_RPC_CLIENT_H_

#include <src/types.h>

#include <string>

using std::string;

namespace libsubtle {
class SearchRequest;
class SearchResponse;

class XmlRpcClient {
 public:
  virtual ~XmlRpcClient() {}
  /// Construct XmlRpcClient
  /// \param user_agent user agent for Service identification.
  /// \param server_endpoint entry point for the Service.
  void Init(const string& user_agent, const string& server_endpoint) {
    user_agent_ = user_agent;
    server_endpoint_ = server_endpoint;
  }

  /// Log in to the service.
  /// \param lang language to use for the session; empty for en_US.
  /// \param req specification of action.
  /// \return response with results.
  virtual LoginResponse LogIn(LoginRequest* req) = 0;

  /// Log out of the service.
  /// \param token of the session to close.
  /// \return response with results.
  virtual LogOutResponse LogOut(const string& token) = 0;

  /// Extend session.
  /// \param token of the session to extend.
  /// \return response with results.
  virtual NoOperationResponse NoOperation(const string& token) = 0;

  /// Find subtitles.
  /// \param token Service authentication token.
  /// \param req specification of action.
  /// \return response with results.
  virtual SearchResponse SearchSubtitles(const string& token,
                                         SearchRequest* req) = 0;
  /// Search and mail subtitles.
  /// \param token Service authentication token.
  /// \param req specification of action.
  /// \return response with results.
  virtual SearchMailResponse SearchMailSubtitles(const string& token,
                                                 SearchMailRequest* req) = 0;
  /// Download subtitles
  /// \param token Service authentication token.
  /// \param req specification of action.
  /// \param response out parameter with results.
  /// \return whether the action succeeded.
  virtual DownloadResponse DownloadSubtitles(const string& token,
                                             DownloadRequest* req) = 0;
  /// Get server info
  /// \return response with results.
  virtual ServerInfoResponse ServerInfo() = 0;

  /// Report wrong movie hash.
  /// \param token Service authentication token.
  /// \param req specification of action.
  /// \return response with results.
  virtual ReportWrongMovieHashResponse ReportWrongMovieHash(
        const string& token, ReportWrongMovieHashRequest* req) = 0;

  /// Vote on a subtitle (1 - 10)
  /// \param token Service authentication token.
  /// \param req specification of action.
  /// \return response with results.
  virtual SubtitlesVoteResponse SubtitlesVote(const string& token,
                                              SubtitlesVoteRequest* req) = 0;

  /// Add comment to the subtitle.
  /// \param token Service authentication token.
  /// \param req specification of action.
  /// \return response with results.
  virtual AddCommentResponse AddComment(const string& token,
                                        AddCommentRequest* req) = 0;

  /// Check movie hash.
  /// \param token Service authentication token.
  /// \param req specification of action.
  /// \return response with results.
  virtual CheckMovieHashResponse CheckMovieHash(const string& token,
                                                CheckMovieHashRequest* req) = 0;

  /// Check subtitle hash.
  /// \param token Service authentication token.
  /// \param req specification of action.
  /// \return response with results.
  virtual CheckSubHashResponse CheckSubHash(const string& token,
                                                CheckSubHashRequest* req) = 0;

  /// Get languages.
  /// \param token Service authentication token.
  /// \param req specification of action.
  /// \return response with results.
  virtual GetSubLanguagesResponse GetSubLanguages(
        GetSubLanguagesRequest* req) = 0;

  /// Detect language.
  /// \param token Service authentication token.
  /// \param req specification of action.
  /// \return response with results.
  virtual DetectLanguageResponse DetectLanguage(const string& token,
                                                DetectLanguageRequest* req) = 0;

  /// Get available translations.
  /// \param token Service authentication token.
  /// \param req specification of action.
  /// \return response with results.
  virtual GetAvailableTranslationsResponse GetAvailableTranslations(
        const string& token, GetAvailableTranslationsRequest* req) = 0;

  /// Get translation.
  /// \param token Service authentication token.
  /// \param req specification of action.
  /// \return response with results.
  virtual GetTranslationResponse GetTranslation(const string& token,
                                                GetTranslationRequest* req) = 0;

  /// Auto update.
  /// \param req specification of action.
  /// \return response with results.
  virtual AutoUpdateResponse AutoUpdate(AutoUpdateRequest* req) = 0;

  /// Search for movies on IMDB.com.
  /// \param token Service authentication token.
  /// \param req specification of action.
  /// \return response with results.
  virtual SearchMoviesOnImdbResponse SearchMoviesOnImdb(const string& token,
       SearchMoviesOnImdbRequest* req) = 0;

  /// Get details about a movie from IMDB.
  /// \param token Service authentication token.
  /// \param req specification of action.
  /// \return response with results.
  virtual GetImdbMovieDetailsResponse GetImdbMovieDetails(const string& token,
        GetImdbMovieDetailsRequest* req) = 0;

  /// Insert movie into the database.
  /// \param token Service authentication token.
  /// \param req specification of action.
  /// \return response with results.
  virtual InsertMovieResponse InsertMovie(const string& token,
        InsertMovieRequest* req) = 0;

 protected:
  /// Agent send with Login req to identify service.
  string user_agent_;
  /// Entry point for the service, a HTTP URI.
  string server_endpoint_;
};

}  // namespace libsubtle

#endif  // SRC_XML_RPC_CLIENT_H_
