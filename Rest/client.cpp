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

namespace uiiit {
namespace rest {

Client::Client(const std::string& aUri)
    : theClient(aUri) {
}

Client::~Client() {
}

std::pair<web::http::status_code, web::json::value>
Client::get(const std::string& aPathQuery) {
  return request(
      web::http::methods::GET, aPathQuery, web::json::value::object());
}

std::pair<web::http::status_code, web::json::value>
Client::post(const web::json::value& aBody, const std::string& aPathQuery) {
  return request(web::http::methods::POST, aPathQuery, aBody);
}

std::pair<web::http::status_code, web::json::value>
Client::put(const web::json::value& aBody, const std::string& aPathQuery) {
  return request(web::http::methods::PUT, aPathQuery, aBody);
}

std::pair<web::http::status_code, web::json::value>
Client::patch(const web::json::value& aBody, const std::string& aPathQuery) {
  return request(web::http::methods::PATCH, aPathQuery, aBody);
}

web::http::status_code Client::del(const std::string& aPathQuery) {
  return request(web::http::methods::DEL, aPathQuery);
}

web::http::status_code Client::request(const web::http::method aMethod,
                                       const std::string&      aPathQuery) {
  web::http::status_code ret;

  theClient.request(aMethod, aPathQuery)
      .then(
          [&ret](web::http::http_response aResp) { ret = aResp.status_code(); })
      .wait();

  return ret;
}

std::pair<web::http::status_code, web::json::value>
Client::request(const web::http::method aMethod,
                const std::string&      aPathQuery,
                const web::json::value& aBody) {
  std::pair<web::http::status_code, web::json::value> ret;

  theClient.request(aMethod, aPathQuery, aBody)
      .then([&ret](web::http::http_response aResp) {
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

} // namespace rest
} // namespace uiiit
