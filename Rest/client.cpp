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

#include "client.h"

#include <glog/logging.h>

#include <cassert>

namespace uiiit {
namespace rest {

Client::Client(const std::string& aUri, const bool aInsecure)
    : theConfig()
    , theClient(nullptr)
    , theUriBuilder()
    , theHeaders() {
  theConfig.set_validate_certificates(not aInsecure);
  theClient = std::make_unique<web::http::client::http_client>(aUri, theConfig);
  // changeHeader("Host",
  //              theClient->base_uri().host() + ":" +
  //                  std::to_string(theClient->base_uri().port()));
}

Client::~Client() {
  assert(theClient != nullptr);
}

void Client::changeHeader(const std::string& aName, const std::string& aValue) {
  theHeaders[aName] = aValue;
}

std::pair<web::http::status_code, web::json::value>
Client::get(const std::string& aPath, const std::string& aQuery) {
  return request(
      web::http::methods::GET, aPath, aQuery, web::json::value::object());
}

std::pair<web::http::status_code, web::json::value>
Client::post(const web::json::value& aBody,
             const std::string&      aPath,
             const std::string&      aQuery) {
  return request(web::http::methods::POST, aPath, aQuery, aBody);
}

std::pair<web::http::status_code, web::json::value>
Client::put(const web::json::value& aBody,
            const std::string&      aPath,
            const std::string&      aQuery) {
  return request(web::http::methods::PUT, aPath, aQuery, aBody);
}

std::pair<web::http::status_code, web::json::value>
Client::patch(const web::json::value& aBody,
              const std::string&      aPath,
              const std::string&      aQuery) {
  return request(web::http::methods::PATCH, aPath, aQuery, aBody);
}

web::http::status_code Client::del(const std::string& aPath,
                                   const std::string& aQuery) {
  return request(web::http::methods::DEL, aPath, aQuery);
}

web::http::status_code Client::request(const web::http::method aMethod,
                                       const std::string&      aPath,
                                       const std::string&      aQuery) {
  web::http::status_code ret;

  // create the HTTP request
  auto myRequest = createRequest(aMethod, aPath, aQuery);
  VLOG(2) << "sending HTTP request to " << theUriBuilder.to_string();

  // send the request and wait for the response
  theClient->request(myRequest)
      .then(
          [&ret](web::http::http_response aResp) { ret = aResp.status_code(); })
      .wait();

  return ret;
}

std::pair<web::http::status_code, web::json::value>
Client::request(const web::http::method aMethod,
                const std::string&      aPath,
                const std::string&      aQuery,
                const web::json::value& aBody) {
  std::pair<web::http::status_code, web::json::value> ret;

  // create the HTTP request
  auto myRequest = createRequest(aMethod, aPath, aQuery);
  myRequest.set_body(aBody);
  VLOG(2) << "sending HTTP request:\n" << myRequest.to_string();

  // send the request and wait for the response
  theClient->request(myRequest)
      .then([&ret](web::http::http_response aResp) {
        VLOG(2) << "received:\n" << aResp.to_string();
        ret.first = aResp.status_code();
        return aResp.extract_json();
      })
      .then([&ret](pplx::task<web::json::value> aPrevTask) {
        try {
          ret.second = aPrevTask.get();
        } catch (web::http::http_exception const& aErr) {
          VLOG(1) << aErr.what();
          throw;
        }
      })
      .wait();

  return ret;
}

web::http::http_request Client::createRequest(const web::http::method aMethod,
                                              const std::string&      aPath,
                                              const std::string&      aQuery) {
  web::http::http_request myRequest(aMethod);
  theUriBuilder.set_path(aPath);
  theUriBuilder.set_query(aQuery);
  myRequest.set_request_uri(theUriBuilder.to_uri());
  for (const auto& elem : theHeaders) {
    myRequest.headers()[elem.first] = elem.second;
  }
  return myRequest;
}

} // namespace rest
} // namespace uiiit
