Library for OpenSubtitles.org
=============================

Subtle is a C++ library for downloading subtitles from [OpenSubtitles.org](http://opensubtitles.org/). It implements [OpenSubtitles.org XmlRpc API](http://trac.opensubtitles.org/projects/opensubtitles/wiki/XMLRPC).
It contains two interfaces, low-level [rpc_impl](https://github.com/stgpetrovic/subtle/blob/master/src/rpc_impl.h) that maps to the OpenSubtitles.org API, and [subtle](https://github.com/stgpetrovic/subtle/blob/master/src/subtle.h) that maps to a higher level API.


Incentive
---------

See src/example.cc. I use the binary like this, in a folder with many video files (think downloads root folder):

    ./subtle eng

It then recurses, finds all video files, and downloads subtitles for them and puts them in the proper place. You can specify any language on the command line to download subtitles for that language. Use [3 letter codes](http://en.wikipedia.org/wiki/List_of_ISO_639-1_codes).

Wanna try it now? Download the 64bit [deb file](https://github.com/stgpetrovic/subtle/raw/master/libsubtle-1.0.0-Linux.deb) or build it from source (make subtle or make package).

User Agent
----------

OpenSubtitles.org require everyone to use an indentifying user agent string in their applications. Subtle uses 'libsubtle', but each application using it should set its own so it can be identified. See more [here](http://trac.opensubtitles.org/projects/opensubtitles/wiki/DevReadFirst).

Low-level interface
===================

Please see [OpenSubtitles.org API specification](http://trac.opensubtitles.org/projects/opensubtitles/wiki/XmlRpcIntro).

Available methods:
  + LogIn, LogOut, NoOperation, ServerInfo
  + SearchSubtitles, SearchMailSubtitles
  + DownloadSubtitles
  + AutoUpdate
  + SubtitlesVote, AddComment
  + GetSubLanguages, DetectLanguage, GetAvailableTranslations, GetTranslation
  + ReportWrongMovieHash, CheckMovieHash, CheckSubHash
  + SearchMoviesOnImbd, GetImbdMovieDetails
  + InsertMovie


Usage
-----

Usage is simple, just include the files and use the API. You can use subtle both as a dynamic library or include it from source, and link appropriately. Please refer to [the example when using subtle from source](https://github.com/stgpetrovic/subtle/blob/master/src/example.cc), or [the example when using subtle as a library](https://github.com/stgpetrovic/subtle/blob/master/src/example_using_library.cc).

The procedure is:
  + Log-in to acquire the session token
  + Execute requests using the token
  + Log-out

Namespace assumed is libsubtle::.
```c++
#include "src/rpc_impl.h"

const string kServerUrl = "http://api.opensubtitles.org/xml-rpc";
const string kUserAgent = "libsubtle";

XmlRpcImpl client;

// log-in
LoginRequest* req = new LoginRequest();
client.Init(kUserAgent, kServerUrl);
LoginResponse res = client_->LogIn(req);
token = res.token_;

// search
SearchRequest* req = new SearchRequest(lng, hash, size);
SearchResponse res = client_->SearchSubtitles(token_, req);
// subtitles in res.subtitles_
```

See the [header](https://github.com/stgpetrovic/subtle/blob/master/src/rpc_impl.h) for all calls and their documentation.

High-level interface
===================

[High-level interface](https://github.com/stgpetrovic/subtle/blob/master/src/subtle.h) is written on top of the low-level one, and offers conveniece methods. Currently it is very poor, but does the important thing - downloads subtitles for a given video file, if available.

Available methods:
  + SearchSubtitles
  + DownloadSubtitles

Usage
-----

Namespace assumed is libsubtle::.
```c++
#include "src/rpc_impl.h"
#include "src/subtle.h"

libsubtle::XmlRpcImpl client;
libsubtle::Subtle s(&client);
s.DownloadSubtitles("eng", "/tmp/Hobbit2_3D.HFR.mkv");
// subtitles downloaded to $PWD
```

See the [header](https://github.com/stgpetrovic/subtle/blob/master/src/subtle.h) for all calls and their documentation.
You can either include "src/subtle.h" or you can link against subtle.so and include "subtle.h", as shown in two examples.

Building
========

Subtle uses CMake. You can do various actions

  + libsubtle           - build the shared library
  + subtle              - build the example that downloads subs recursivley for all videos in subfolders
  + cov                 - generate test coverage (requires lcov package installed)
  + doc                 - generate documentation (requires doxygen package installed)
  + package             - generate debian package of the shared library and the subtle binary
  + test                - run tests
  + example_using_lib   - build the example that includes subtle as a library
  + all

Current tests work against the live server, they don't have a mock one nor plan to so I don't care. Coverage is 93%.


Dependencies
============

Subtle depends on a few libraries:
  + libxmlrpc-c++       - implements the xml rpc protocol used by OpenSubtitles.org
  + libzip              - because subtitles are streamed zipped

Please satify these dependencies on your distribution (varies).

    Ubuntu: apt-get install libzip-dev libxmlrpc-c++-dev lcov
    Gentoo: emerge libzip xmlrpc-c lcov

Credits
=======

I am not a C++ programmer so please if you find something horribly wrong with it, send me a pull request.
