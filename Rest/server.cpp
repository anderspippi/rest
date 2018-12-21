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
