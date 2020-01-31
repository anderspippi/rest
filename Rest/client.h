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

#include <cpprest/http_client.h>
#include <cpprest/json.h>
#include <cpprest/uri_builder.h>

#include <map>
#include <memory>
#include <string>
#include <utility>

namespace uiiit {
namespace rest {

/**
 * Basic HTTP client.
 *
 * Calls to the methods of this class are not thread-safe.
 */
class Client
{
 public:
  /**
   * Create an HTTP client.
   *
   * \param aUri the base URI to call methods.
   *
   * \param aInsecure if false accept also invalid SSL certificates.
   */
  explicit Client(const std::string& aUri, const bool aInsecure = false);
  virtual ~Client();

  //! Add/modify a given header to future requests.
  void changeHeader(const std::string& aName, const std::string& aValue);

  std::pair<web::http::status_code, web::json::value>
  get(const std::string& aPath = std::string(), const std::string& aQuery = std::string());

  std::pair<web::http::status_code, web::json::value>
  post(const web::json::value& aBody,
       const std::string&      aPath = std::string(),
       const std::string&      aQuery = std::string());

  std::pair<web::http::status_code, web::json::value>
  put(const web::json::value& aBody,
      const std::string&      aPath = std::string(),
      const std::string&      aQuery = std::string());

  std::pair<web::http::status_code, web::json::value>
  patch(const web::json::value& aBody,
        const std::string&      aPath = std::string(),
        const std::string&      aQuery = std::string());

  web::http::status_code del(const std::string& aPath = std::string(),
                             const std::string& aQuery = std::string());

 private:
  std::pair<web::http::status_code, web::json::value>
  request(const web::http::method aMethod,
          const std::string&      aPath,
          const std::string&      aQuery,
          const web::json::value& aBody);

  web::http::status_code request(const web::http::method aMethod,
                                 const std::string&      aPath,
                                 const std::string&      aQuery);

  web::http::http_request createRequest(const web::http::method aMethod,
                                        const std::string&      aPath,
                                        const std::string&      aQuery);

 private:
  web::http::client::http_client_config           theConfig;
  std::unique_ptr<web::http::client::http_client> theClient;
  web::uri_builder                                theUriBuilder;
  std::map<std::string, std::string>              theHeaders;
};

} // namespace rest
} // namespace uiiit
