/*
 ___ ___ __     __ ____________
|   |   |  |   |__|__|__   ___/  Ubiquitout Internet @ IIT-CNR
|   |   |  |  /__/  /  /  /      REST wrapper around cpprest
|   |   |  |/__/  /   /  /       https://bitbucket.org/ccicconetti/rest/
|_______|__|__/__/   /__/

Licensed under the MIT License <http://opensource.org/licenses/MIT>.
Copyright (c) 2018 Claudio Cicconetti <https://about.me/ccicconetti>

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

#include "Support/thrower.h"

#include <cpprest/json.h>

#include <string>

namespace uiiit {
namespace rest {

/**
 * \param aObject The JSON object where to add aName.
 * \param aName The field name.
 * \param aValue The value of the field to be added/modified.
 * \param aMayOverride Never override an existing value if false.
 *
 * \return true if the operation has been done, false otherwise.
 *
 * \throw std::runtime_error if aValue is not a JSON object.
 */
bool setIfNotEmpty(web::json::value&  aObject,
                   const std::string& aName,
                   const std::string& aValue,
                   const bool         aMayOverride);

//! \return the value of the given field, if present, else an empty string.
std::string stringOrEmpty(const web::json::value& aObject,
                          const std::string&      aField);

//! Raise error if the given field is empty, but only if present.
void notEmptyIfPresent(const web::json::value& aJson,
                       const std::string&      aField,
                       support::Thrower&       aThrower);

//! Raise error if the given field is not present or empty.
void notEmpty(const web::json::value& aJson,
              const std::string&      aField,
              support::Thrower&       aThrower);

//! Raise error if the string is present and exceeds the given length.
void tooLong(const web::json::value& aJson,
             const std::string&      aField,
             const size_t            aMaxSize,
             support::Thrower&       aThrower);

//! Raise an error if the given field is present but does not contain a number.
void notNumberIfPresent(const web::json::value& aJson,
                        const std::string&      aField,
                        support::Thrower&       aThrower);

} // namespace rest
} // namespace uiiit
