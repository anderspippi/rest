/*
 ___ ___ __     __ ____________
|   |   |  |   |__|__|__   ___/  Ubiquitout Internet @ IIT-CNR
|   |   |  |  /__/  /  /  /      REST wrapper around cpprest
|   |   |  |/__/  /   /  /       https://github.com/ccicconetti/rest/
|_______|__|__/__/   /__/

Licensed under the MIT License <http://opensource.org/licenses/MIT>.
Copyright (c) 2019 Claudio Cicconetti https://ccicconetti.github.io/

Permission is hereby  granted, free of charge, to any  person obtaining a copy
of this software and associated  documentation files (the "Software"), to deal
in the Software  without restriction, including without  limitation the rights
to  use, copy,  modify, merge,  publish, distribute,  sublicense, and/or  sell
copies  of  the Software,  and  to  permit persons  to  whom  the Software  is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE  IS PROVIDED "AS  IS", WITHOUT WARRANTY  OF ANY KIND,  EXPRESS OR
IMPLIED,  INCLUDING BUT  NOT  LIMITED TO  THE  WARRANTIES OF  MERCHANTABILITY,
FITNESS FOR  A PARTICULAR PURPOSE AND  NONINFRINGEMENT. IN NO EVENT  SHALL THE
AUTHORS  OR COPYRIGHT  HOLDERS  BE  LIABLE FOR  ANY  CLAIM,  DAMAGES OR  OTHER
LIABILITY, WHETHER IN AN ACTION OF  CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE  OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "server.h"

#include "Rest/problemdetails.h"

#include <glog/logging.h>

#include <regex>

namespace uiiit {
namespace rest {

Server::Server(const std::string& aUri)
    : theListener(aUri)
    , theStarted(false)
    , theHandlers() {
  theListener.support(
      [this](web::http::http_request aReq) { this->handle(aReq); });
}

void Server::start() {
  theListener.open()
      .then([this]() {
        LOG(INFO) << "REST server started at " << theListener.uri().to_string();
      })
      .wait();
  theStarted = true;
}

Server::~Server() {
  if (theStarted) {
    theListener.close()
        .then([]() { LOG(INFO) << "REST server stopped"; })
        .wait();
  }
}

void Server::handle(web::http::http_request aReq) {
  if (not VLOG_IS_ON(2)) {
    VLOG(1) << aReq.method() << ' ' << aReq.absolute_uri().to_string();
  }
  VLOG(2) << aReq.to_string();

  web::http::status_code myStatus{web::http::status_codes::NotFound};
  try {
    const auto it = theHandlers.find(aReq.method());
    if (it != theHandlers.end()) {
      for (const auto& elem : it->second) {
        if (std::regex_match(aReq.relative_uri().path(),
                             std::regex(elem.first))) {
          elem.second(aReq);
          return;
        }
      }
    }
  } catch (...) {
    myStatus = web::http::status_codes::BadRequest;
  }

  web::json::value myRet;
  myRet["ProblemDetails"] =
      ProblemDetails(myStatus,
                     "Error processing method " + aReq.method() + " at " +
                         aReq.absolute_uri().to_string())
          .toJson();
  aReq.reply(myStatus, myRet);
}

void Server::operator()(const web::http::method& aMethod,
                        const std::string&       aRegex,
                        const Handler&           aHandler) {
  std::ignore = std::regex(aRegex); // throw if invalid

  const auto it = theHandlers.emplace(aMethod, HandlersMap::mapped_type());
  assert(it.first != theHandlers.end());
  const auto jt = it.first->second.emplace(aRegex, aHandler);
  LOG_IF(WARNING, not jt.second)
      << "Trying to override a method/regex with a handler: ignored";
}

} // namespace rest
} // namespace uiiit
