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
