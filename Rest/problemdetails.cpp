#include "problemdetails.h"

#include "Rest/utils.h"
#include "Support/tostring.h"

#include <cassert>

namespace uiiit {
namespace rest {

ProblemDetails::ProblemDetails(const std::string&           aType,
                               const std::string&           aTitle,
                               const web::http::status_code aStatus,
                               const std::string&           aDetail,
                               const std::string&           aInstance)
    : DataType() {
  rest::setIfNotEmpty(theObject, "type", aType, false);
  rest::setIfNotEmpty(theObject, "title", aTitle, false);
  theObject["status"] = web::json::value(aStatus);
  rest::setIfNotEmpty(theObject, "detail", aDetail, false);
  rest::setIfNotEmpty(theObject, "instance", aInstance, false);
}

ProblemDetails::ProblemDetails(const web::http::status_code aStatus,
                               const std::string&           aDetail)
    : ProblemDetails("", "", aStatus, aDetail, "") {
}

ProblemDetails::ProblemDetails(const web::json::value& aJson)
    : DataType(aJson) {
  std::string myErr;
  if (not aJson.has_integer_field("status")) {
    myErr += " status";
  }
  if (not aJson.has_string_field("detail")) {
    myErr += " detail";
  }
  if (not myErr.empty()) {
    throw std::runtime_error(
        "Error creating a ProblemDetails without required fields: " + myErr);
  }
}

std::string ProblemDetails::type() const {
  return theObject.has_field("type") ?
             theObject.as_object().at("type").as_string() :
             std::string();
}

std::string ProblemDetails::title() const {
  return theObject.has_field("title") ?
             theObject.as_object().at("title").as_string() :
             std::string();
}

web::http::status_code ProblemDetails::status() const {
  assert(theObject.has_field("status"));
  return theObject.as_object().at("status").as_integer();
}

std::string ProblemDetails::detail() const {
  assert(theObject.has_field("detail"));
  return theObject.as_object().at("detail").as_string();
}

std::string ProblemDetails::instance() const {
  return theObject.has_field("instance") ?
             theObject.as_object().at("instance").as_string() :
             std::string();
}

std::string safeProblemDetailsString(const web::json::value& aJson) noexcept {
  try {
    return toString(ProblemDetails(aJson));
  } catch (...) {
  }
  return "invalid ProblemDetails";
}

} // namespace rest
} // namespace uiiit

std::ostream& operator<<(std::ostream&                      aStream,
                         const uiiit::rest::ProblemDetails& aProblemDetails) {
  const auto myType     = aProblemDetails.type();     // can be empty
  const auto myTitle    = aProblemDetails.title();    // can be empty
  const auto myStatus   = aProblemDetails.status();   // cannot be empty
  const auto myDetail   = aProblemDetails.detail();   // cannot be empty
  const auto myInstance = aProblemDetails.instance(); // can be empty

  auto myFirst = true;
  if (not myType.empty()) {
    aStream << "type: " << myType;
    myFirst = false;
  }
  if (not myTitle.empty()) {
    if (not myFirst) {
      aStream << ", ";
    }
    aStream << "title: " << myTitle;
    myFirst = false;
  }
  if (not myFirst) {
    aStream << ", ";
  }
  aStream << "status: " << myStatus << ", "
          << "detail: " << myDetail;
  if (not myInstance.empty()) {
    aStream << ", instance: " << myInstance;
  }

  return aStream;
}
