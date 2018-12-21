#pragma once

#include <cpprest/http_client.h>
#include <cpprest/json.h>

#include <string>
#include <utility>

namespace uiiit {
namespace rest {

class Client
{
 public:
  explicit Client(const std::string& aUri);
  virtual ~Client();

  std::pair<web::http::status_code, web::json::value>
  get(const std::string& aPathQuery = std::string());

  std::pair<web::http::status_code, web::json::value>
  post(const web::json::value& aBody,
       const std::string&      aPathQuery = std::string());

  std::pair<web::http::status_code, web::json::value>
  put(const web::json::value& aBody,
      const std::string&      aPathQuery = std::string());

  std::pair<web::http::status_code, web::json::value>
  patch(const web::json::value& aBody,
        const std::string&      aPathQuery = std::string());

  web::http::status_code del(const std::string& aPathQuery = std::string());

 private:
  std::pair<web::http::status_code, web::json::value>
  request(const web::http::method aMethod,
          const std::string&      aPathQuery,
          const web::json::value& aBody);

  web::http::status_code request(const web::http::method aMethod,
                                 const std::string&      aPathQuery);

 private:
  web::http::client::http_client theClient;
};

} // namespace rest
} // namespace uiiit
