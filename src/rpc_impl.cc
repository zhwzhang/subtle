#include <src/rpc_impl.h>
#include <src/types.h>
#include <src/subfile.h>

#include <algorithm>
#include <iostream>
#include <map>
#include <string>
#include <utility>
#include <vector>

using std::make_pair;
using std::map;
using std::string;
using std::vector;

using xmlrpc_c::value;
using xmlrpc_c::value_array;
using xmlrpc_c::value_double;
using xmlrpc_c::value_int;
using xmlrpc_c::value_string;
using xmlrpc_c::value_struct;

namespace libsubtle {

typedef map<string, value> StructDict;

string ds(value v) {
    return static_cast<string>(value_string(v));
}
string s(StructDict v, string k) {
  if (v.find(k) != v.end()) {
    return ds(v[k]);
  } else {
    return "";
  }
}

int64_t i(StructDict v, string k) {
  return static_cast<int64_t>(value_int(v[k]));
}

double d(StructDict v, string k) {
  return static_cast<double>(value_double(v[k]));
}

StructDict v(value val) {
  return static_cast<StructDict>(value_struct(val));
}

bool CheckResponse(StructDict values) {
  return s(values, "status").compare("200 OK") == 0;
}

void ToStrMap(StructDict value_dict, string field, map<string,
    string>* target) {
  if (value_dict.find(field) != value_dict.end()) {
    StructDict dict = v(value_dict[field]);
    for (StructDict::const_iterator iter = dict.begin();
        iter != dict.end(); ++iter) {
      (*target)[iter->first] = ds(iter->second);
    }
  }
}

void ToStrVector(StructDict value_dict, string field, vector<string>* target) {
  if (value_dict.find(field) != value_dict.end()) {
    value data_field = value_dict[field];
    vector<value> const data(
        value_array(data_field).vectorValueValue());
    for (const auto& entry : data) {
      target->push_back(ds(entry));
    }
  }
}

XmlRpcImpl::~XmlRpcImpl() {
}

extern "C" LoginResponse XmlRpcImpl::LogIn(LoginRequest* request) {
  value result;
  client_.call(server_endpoint_, "LogIn", "ssss", &result,
               request->username_.c_str(), request->password_.c_str(),
               request->lang_.c_str(), user_agent_.c_str());

  StructDict values = v(result);
  LoginResponse response;
  response.SetStatus(s(values, "status"), d(values, "seconds"));
  if (CheckResponse(values)) {
    response.token_ = s(values, "token");
  }

  return response;
}

extern "C" LogOutResponse XmlRpcImpl::LogOut(const string& token) {
  LogOutResponse response;
  value result;
  client_.call(server_endpoint_, "LogOut", "s", &result, token.c_str());
  StructDict values = v(result);
  response.SetStatus(s(values, "status"), d(values, "seconds"));
  return response;
}

extern "C" NoOperationResponse XmlRpcImpl::NoOperation(const string& token) {
  NoOperationResponse response;
  value result;
  client_.call(server_endpoint_, "NoOperation", "s", &result, token.c_str());
  StructDict values = v(result);
  response.SetStatus(s(values, "status"), d(values, "seconds"));
  return response;
}

extern "C" SearchResponse XmlRpcImpl::SearchSubtitles(const string& token,
                                                      SearchRequest* request) {
  SearchResponse response;
  xmlrpc_c::paramList param_list;
  param_list.add(value_string(token));
  map<string, value> req_map;

  // imdb or hash search?
  if (request->imdb_id_.compare("")) {
    req_map.insert(make_pair("imdbid", value_string(request->imdb_id_)));
  } else {
    req_map.insert(make_pair("moviehash", value_string(request->movie_hash_)));
    req_map.insert(make_pair("moviebytesize",
                             value_double(request->movie_byte_size_)));
  }
  req_map.insert(make_pair("sublanguageid",
                            value_string(request->sub_language_id_)));

  value_struct const request_value(req_map);
  vector<value> params;
  params.push_back(request_value);
  value_array params_array(params);
  param_list.add(params_array);

  value result;
  client_.call(server_endpoint_, "SearchSubtitles", param_list, &result);
  StructDict values = v(result);
  response.SetStatus(s(values, "status"), d(values, "seconds"));

  // False is returned when there's no results
  if (response.GetStatus() != OK ||
      values["data"].type() == value::TYPE_BOOLEAN) {
    return response;
  }

  vector<SubFile> ret;
  value data_field = values["data"];

  // parse results
  vector<value> const data(
      value_array(data_field).vectorValueValue());

  for (vector<value>::const_iterator it =  data.begin();
        it != data.end(); ++it) {
    map<string, string> members;
    StructDict values = v(*it);

    for (StructDict::const_iterator iter = values.begin();
        iter != values.end(); ++iter) {
      try {
        members[iter->first] = ds(value_string(iter->second));
      } catch(...) {
        // ignore non-strings from the response
      }
    }

      ret.push_back(SubFile(members));
    }

  response.data_.swap(ret);

  return response;
}

extern "C" SearchMailResponse XmlRpcImpl::SearchMailSubtitles(
          const string& token,
          SearchMailRequest* request) {
    SearchMailResponse response;
    xmlrpc_c::paramList param_list;

    // fill in language array
    vector<value> lang_array;
    for (vector<string>::const_iterator it = request->languages_.begin();
        it != request->languages_.end(); ++it) {
      lang_array.push_back(value_string(*it));
    }
    value_array lang_list(lang_array);

    // fill in movie data
    vector<value> movie_array;
    vector<pair<string, double> >::const_iterator it;
    for (it = request->movies_.begin(); it != request->movies_.end(); ++it) {
      StructDict d;
      d["moviehash"] = value_string(it->first);
      d["moviesize"] = value_double(it->second);
      movie_array.push_back(value_struct(d));
    }
    value_array movie_list(movie_array);

    param_list.add(value_string(token));
    param_list.add(lang_list);
    param_list.add(movie_list);

    value result;
    client_.call(server_endpoint_, "SearchToMail", param_list, &result);
    StructDict values = v(result);

    response.SetStatus(s(values, "status"), d(values, "seconds"));

    return response;
}

extern "C" DownloadResponse XmlRpcImpl::DownloadSubtitles(
            const string& token,
            DownloadRequest* request) {
    DownloadResponse response;
    xmlrpc_c::paramList param_list;

    // fill in movie data
    vector<value> movie_data(request->movies_.size());
    std::transform(request->movies_.begin(), request->movies_.end(),
                   movie_data.begin(), [](int id){return value_int(id);});
    value_array movie_list(movie_data);

    param_list.add(value_string(token));
    param_list.add(movie_list);

    value result;
    client_.call(server_endpoint_, "DownloadSubtitles", param_list, &result);
    StructDict values = v(result);

    response.SetStatus(s(values, "status"), d(values, "seconds"));

    if (response.GetStatus() == OK) {
      value data_field = values["data"];
      vector<value> const data(
          value_array(data_field).vectorValueValue());
      for (vector<value>::const_iterator it =  data.begin();
           it != data.end(); ++it) {
        StructDict subtitles = v(*it);

        string sub_id = s(subtitles, "idsubtitlefile");
        string sub_base64 = s(subtitles, "data");

        response.subtitles_.push_back(make_pair(sub_id, sub_base64));
      }
    }

    return response;
}

extern "C" ServerInfoResponse XmlRpcImpl::ServerInfo() {
  value result;
  client_.call(server_endpoint_, "ServerInfo", &result);
  StructDict values = v(result);

  ServerInfoResponse response;
  response.xmlrpc_version_ = s(values, "xmlrpc_version");
  response.xmlrpc_url_ = s(values, "xmlrpc_url");
  response.application_ = s(values, "application");
  response.contact_ = s(values, "contact");
  response.website_url_ = s(values, "website_url");
  response.users_online_total_ = i(values, "users_online_total");
  response.users_online_program_ = i(values, "users_online_program");
  response.users_loggedin_ = i(values, "users_loggedin");
  response.users_max_alltime_ = s(values, "users_max_alltime");
  response.users_registered_ = s(values, "users_registered");
  response.subs_downloads_ = s(values, "subs_downloads");
  response.subs_subtitle_files_ = s(values, "subs_subtitle_files");
  response.movies_total_ = s(values, "movies_total");
  response.movies_aka_ = s(values, "movies_aka");
  response.total_subtitles_languages_ = s(values, "total_subtitles_languages");

  StructDict langs = v(values["last_update_strings"]);

  for (StructDict::const_iterator iter = langs.begin();
       iter != langs.end(); ++iter) {
    response.last_update_strings_[iter->first] = ds(iter->second);
  }

  return response;
}

extern "C" ReportWrongMovieHashResponse XmlRpcImpl::ReportWrongMovieHash(
        const string& token, ReportWrongMovieHashRequest* request) {
  value result;
  client_.call(server_endpoint_, "ReportWrongMovieHash", "si", &result,
               token.c_str(), request->id_sub_movie_file_);
  StructDict values = v(result);
  ReportWrongMovieHashResponse response;
  response.SetStatus(s(values, "status"), d(values, "seconds"));

  return response;
}

extern "C" SubtitlesVoteResponse XmlRpcImpl::SubtitlesVote(
         const string& token, SubtitlesVoteRequest* req) {
  value result;
  SubtitlesVoteResponse response;

  xmlrpc_c::paramList param_list;
  param_list.add(value_string(token));
  map<string, value> req_map;
  req_map.insert(make_pair("idsubtitle", value_int(
            req->id_subtitle_)));
  req_map.insert(make_pair("score", value_int(
            req->score_)));
  value_struct const request_value(req_map);
  param_list.add(request_value);

  client_.call(server_endpoint_, "SubtitlesVote", param_list, &result);
  StructDict values = v(result);
  response.SetStatus(s(values, "status"), d(values, "seconds"));

  return response;
}

AddCommentResponse XmlRpcImpl::AddComment(const string& token,
                                          AddCommentRequest* req) {
  value result;
  AddCommentResponse response;

  xmlrpc_c::paramList param_list;
  param_list.add(value_string(token));
  map<string, value> req_map;
  req_map.insert(make_pair("idsubtitle", value_int(req->id_subtitle_)));
  req_map.insert(make_pair("comment", value_string(req->comment_)));
  req_map.insert(make_pair("badsubtitle", value_int(req->bad_subtitle_)));
  value_struct const request_value(req_map);
  param_list.add(request_value);

  client_.call(server_endpoint_, "AddComment", param_list, &result);
  StructDict values = v(result);
  response.SetStatus(s(values, "status"), d(values, "seconds"));

  return response;
}

CheckMovieHashResponse XmlRpcImpl::CheckMovieHash(const string& token,
                                                  CheckMovieHashRequest* req) {
  CheckMovieHashResponse response;
  xmlrpc_c::paramList param_list;
  param_list.add(value_string(token));

  vector<value> movie_hashes;
  for (const auto& hash : req->movie_hashes_) {
    movie_hashes.push_back(value_string(hash));
  }
  param_list.add(value_array(movie_hashes));

  value result;
  client_.call(server_endpoint_, "CheckMovieHash", param_list, &result);
  StructDict values = v(result);
  response.SetStatus(s(values, "status"), d(values, "seconds"));

  StructDict data_field = v(values["data"]);
  for (StructDict::const_iterator iter = data_field.begin();
       iter != data_field.end(); ++iter) {
    StructDict info_field = v(iter->second);

    response.movie_infos_.insert(make_pair(iter->first, MovieInfo(
        s(info_field, "MovieHash"), s(info_field, "MovieImdbID"),
        s(info_field, "MovieName"), s(info_field, "MovieYear"))));
  }

  return response;
}

CheckSubHashResponse XmlRpcImpl::CheckSubHash(const string& token,
                                              CheckSubHashRequest* req) {
  CheckSubHashResponse response;
  xmlrpc_c::paramList param_list;
  param_list.add(value_string(token));

  vector<value> sub_hashes;
  for (const auto& hash : req->sub_hashes_) {
    sub_hashes.push_back(value_string(hash));
  }
  param_list.add(value_array(sub_hashes));

  value result;
  client_.call(server_endpoint_, "CheckSubHash", param_list, &result);
  StructDict values = v(result);
  response.SetStatus(s(values, "status"), d(values, "seconds"));

  StructDict data_field = v(values["data"]);
  for (StructDict::const_iterator iter = data_field.begin();
       iter != data_field.end(); ++iter) {
    // work around crap in the API that returns int(0) when sub is not present
    // and string(<sub_id>) when it is. Gotta love fucking php.
    string sub_id;
    if (iter->second.type() == value::TYPE_INT) {
      sub_id = std::to_string(value_int(iter->second));
    } else {
      sub_id = value_string(iter->second);
    }
    response.sub_ids_[iter->first] = value_string(sub_id);
  }

  return response;
}

GetSubLanguagesResponse XmlRpcImpl::GetSubLanguages(
    GetSubLanguagesRequest* req) {
  GetSubLanguagesResponse response;
  value result;
  client_.call(server_endpoint_, "GetSubLanguages", "s", &result,
               req->lang_.c_str());
  StructDict values = v(result);
  // seems like this method does not return status... consistent.
  response.SetStatus("200 OK", d(values, "seconds"));
  value data_field = values["data"];

  // parse results
  vector<value> const data(
      value_array(data_field).vectorValueValue());

  for (vector<value>::const_iterator it =  data.begin();
        it != data.end(); ++it) {
    StructDict info_field = v(*it);

    response.lang_infos_.push_back(LangInfo(
        s(info_field, "SubLanguageID"), s(info_field, "LanguageName"),
        s(info_field, "ISO639")));
  }

  return response;
}

DetectLanguageResponse XmlRpcImpl::DetectLanguage(const string& token,
                                                  DetectLanguageRequest* req) {
  DetectLanguageResponse response;
  xmlrpc_c::paramList param_list;
  param_list.add(value_string(token));

  vector<value> movie_hashes;
  for (const auto& hash : req->texts_) {
    movie_hashes.push_back(value_string(hash));
  }
  param_list.add(value_array(movie_hashes));

  value result;
  client_.call(server_endpoint_, "DetectLanguage", param_list, &result);
  StructDict values = v(result);
  response.SetStatus(s(values, "status"), d(values, "seconds"));

  StructDict data_field = v(values["data"]);
  for (StructDict::const_iterator iter = data_field.begin();
       iter != data_field.end(); ++iter) {
    response.detected_langs_.insert(make_pair(value_string(iter->first),
                                              value_string(iter->second)));
  }

  return response;
}

GetAvailableTranslationsResponse XmlRpcImpl::GetAvailableTranslations(
    const string& token, GetAvailableTranslationsRequest* req) {
  GetAvailableTranslationsResponse response;
  value result;
  client_.call(server_endpoint_, "GetAvailableTranslations", "ss", &result,
               token.c_str(), req->program_.c_str());
  StructDict values = v(result);
  // seems like this method does not return status... consistent.
  response.SetStatus("200 OK", d(values, "seconds"));
  StructDict data = v(values["data"]);

  for (const auto& trans : data) {
    StructDict info_field = v(trans.second);

    response.translation_infos_.insert(make_pair(
        static_cast<string>(trans.first),
        TranslationInfo(s(info_field, "LastCreated"),
                        s(info_field, "StringsNo"))));
  }

  return response;
}

GetTranslationResponse XmlRpcImpl::GetTranslation(const string& token,
                                                  GetTranslationRequest* req) {
  GetTranslationResponse response;
  value result;
  client_.call(server_endpoint_, "GetTranslation", "ssss", &result,
               token.c_str(), req->iso639_.c_str(), req->format_.c_str(),
               req->program_.c_str());
  StructDict values = v(result);
  response.SetStatus(s(values, "status"), d(values, "seconds"));
  if (!CheckResponse(values)) {
    throw SubtleException(response.StatusMessage());
  }
  response.translation_ = s(values, "data");

  return response;
}

AutoUpdateResponse XmlRpcImpl::AutoUpdate(AutoUpdateRequest* req) {
  value result;
  client_.call(server_endpoint_, "AutoUpdate", "s", &result,
               req->program_.c_str());
  StructDict values = v(result);

  // only one OS linx might be present
  string windows, linux, comments;
  if (values.find("url_windows") != values.end()) {
    windows = s(values, "url_windows");
  }
  if (values.find("url_linux") != values.end()) {
    linux = s(values, "url_linux");
  }
  if (values.find("comments") != values.end()) {
    comments = s(values, "comments");
  }
  AutoUpdateResponse response;
  response.version_ = s(values, "version");
  response.url_windows_ = windows;
  response.url_linux_ = linux;
  response.comments_ = comments;
  response.SetStatus(s(values, "status"), d(values, "seconds"));

  return response;
}

SearchMoviesOnImdbResponse XmlRpcImpl::SearchMoviesOnImdb(const string& token,
    SearchMoviesOnImdbRequest* req) {
  SearchMoviesOnImdbResponse response;
  value result;
  client_.call(server_endpoint_, "SearchMoviesOnIMDB", "ss", &result,
               token.c_str(), req->query_.c_str());
  StructDict values = v(result);
  response.SetStatus(s(values, "status"), d(values, "seconds"));

  value data_field = values["data"];

  // parse results
  vector<value> const data(
      value_array(data_field).vectorValueValue());

  for (vector<value>::const_iterator it =  data.begin();
        it != data.end(); ++it) {
    StructDict info_field = v(*it);

    response.imdb_results_.push_back(ImdbEntry {
        s(info_field, "id"), s(info_field, "title")});
  }

  return response;
}

GetImdbMovieDetailsResponse XmlRpcImpl::GetImdbMovieDetails(const string& token,
        GetImdbMovieDetailsRequest* req) {
  GetImdbMovieDetailsResponse response;
  value result;
  client_.call(server_endpoint_, "GetIMDBMovieDetails", "ss", &result,
               token.c_str(), req->imdb_id_.c_str());
  StructDict values = v(result);
  response.SetStatus(s(values, "status"), d(values, "seconds"));

  StructDict data_field = v(values["data"]);

  // simple fields
  try {
    response.id_ = s(data_field, "id");
    response.title_ = s(data_field, "title");
    response.year_ = s(data_field, "year");
    response.cover_ = s(data_field, "cover");
    response.awards_ = s(data_field, "awards");
    response.duration_ = s(data_field, "duration");
    response.tagline_ = s(data_field, "tagline");
    response.plot_ = s(data_field, "plot");
    response.goofs_ = s(data_field, "goofs");
    response.trivia_ = s(data_field, "trivia");
    response.request_from_ = s(data_field, "request_from");

    // maps
    ToStrMap(data_field, "cast", &response.cast_);
    ToStrMap(data_field, "directors", &response.directors_);
    ToStrMap(data_field, "writers", &response.writers_);

    // arrays
    ToStrVector(data_field, "genres", &response.genres_);
    ToStrVector(data_field, "countries", &response.countries_);
    ToStrVector(data_field, "languages", &response.languages_);
  } catch(...) {
      // kitten dies, but this php api just randomly returns crap from scraping
      // IMDb (e.g. javascript code), as the site changes.
  }

  return response;
}

InsertMovieResponse XmlRpcImpl::InsertMovie(const string& token,
                                            InsertMovieRequest* req) {
  InsertMovieResponse response;
  xmlrpc_c::paramList param_list;
  param_list.add(value_string(token));
  map<string, value> req_map;

  req_map.insert(make_pair("moviename", value_string(req->movie_name_)));
  req_map.insert(make_pair("movieyear", value_string(req->movie_year_)));

  value_struct const request_value(req_map);
  param_list.add(request_value);

  value result;
  client_.call(server_endpoint_, "InsertMovie", param_list, &result);
  StructDict values = v(result);
  response.SetStatus(s(values, "status"), d(values, "seconds"));
  response.id_ = s(values, "id");

  return response;
}

}  // namespace libsubtle
