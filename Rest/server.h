#pragma once

#include "Support/macros.h"

#include <cpprest/http_listener.h>

#include <functional>
#include <map>
#include <string>

namespace uiiit {
namespace rest {

class Server
{
  NONCOPYABLE_NONMOVABLE(Server);

 public:
  using Handler = std::function<void(web::http::http_request)>;

  /**
   * Create a REST server that will listen to the given URI.
   * The server is not started upon creation.
   */
  explicit Server(const std::string& aUri);

  virtual ~Server();

  //! Start the server.
  void start();

 protected:
  //! Add a handler for a given method and regex.
  void operator()(const web::http::method& aMethod,
                  const std::string&       aRegex,
                  const Handler&           aHandler);

 private:
  void handle(web::http::http_request aReq);

 private:
  using HandlersMap =
      std::map<web::http::method, std::map<std::string, Handler>>;

  web::http::experimental::listener::http_listener theListener;
  bool                                             theStarted;
  HandlersMap                                      theHandlers;
};

} // namespace rest
} // namespace uiiit
