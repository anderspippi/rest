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
