#include "utils.h"

#include <stdexcept>

namespace uiiit {
namespace rest {

bool setIfNotEmpty(web::json::value&  aObject,
                   const std::string& aName,
                   const std::string& aValue,
                   const bool         aMayOverride) {
  if (not aObject.is_object()) {
    throw std::runtime_error("Invalid operation on a non-object JSON value: " +
                             aObject.serialize());
  }
  auto& myObject = aObject.as_object();
  if (aValue.empty()) {
    return true;
  }
  const auto it = myObject.find(aName);
  if (it != myObject.end() and not aMayOverride) {
    return false;
  }
  myObject[aName] = web::json::value(aValue);
  return true;
}

std::string stringOrEmpty(const web::json::value& aObject,
                          const std::string&      aField) {
  return aObject.has_string_field(aField) ?
             aObject.as_object().at(aField).as_string() :
             std::string();
}

void notEmptyIfPresent(const web::json::value& aJson,
                       const std::string&      aField,
                       support::Thrower&       aThrower) {
  aThrower(aJson.has_string_field(aField) and
               aJson.at(aField).as_string().empty(),
           "empty " + aField);
}

void notEmpty(const web::json::value& aJson,
              const std::string&      aField,
              support::Thrower&       aThrower) {
  aThrower(not aJson.has_string_field(aField) or
               aJson.at(aField).as_string().empty(),
           "empty/missing " + aField);
}

void tooLong(const web::json::value& aJson,
             const std::string&      aField,
             const size_t            aMaxSize,
             support::Thrower&       aThrower) {
  aThrower(aJson.has_string_field(aField) and
               aJson.at(aField).as_string().size() > aMaxSize,
           aField + " exceeds " + std::to_string(aMaxSize) + " characters");
}

void notNumberIfPresent(const web::json::value& aJson,
                        const std::string&      aField,
                        support::Thrower&       aThrower) {
  aThrower(aJson.has_field(aField) and not aJson.at(aField).is_number(),
           "field " + aField + " is not a number");
}

} // namespace rest
} // namespace uiiit
