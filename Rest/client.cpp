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
