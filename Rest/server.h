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
