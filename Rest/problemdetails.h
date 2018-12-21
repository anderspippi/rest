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

#include "Rest/datatype.h"

#include <cpprest/http_msg.h>

#include <iostream>
#include <string>

namespace uiiit {
namespace rest {

/**
 * Problem Details object, as defined in IETF RFC 7807.
 */
class ProblemDetails : public DataType
{

 public:
  /**
   * A full Problem Details.
   *
   * \param aType A URI reference [RFC3986] that identifies the problem type.
   * \param aTitle A short, human-readable summary of the problem type.
   * \param aStatus The HTTP status code.
   * \param aDetail A human-readable explanation specific to this occurrence
   *        of the problem.
   * \param aInstance A URI reference that identifies the specific occurrence of
   *        the problem.
   */
  explicit ProblemDetails(const std::string&           aType,
                          const std::string&           aTitle,
                          const web::http::status_code aStatus,
                          const std::string&           aDetail,
                          const std::string&           aInstance);

  /**
   * A minimal Problem Details object with required fields.
   */
  explicit ProblemDetails(const web::http::status_code aStatus,
                          const std::string&           aDetail);

  /**
   * Problem Details from a JSON.
   *
   * \throws std::runtime_error if the required fields (status and detail) are
   * not present.
   */
  explicit ProblemDetails(const web::json::value& aJson);

  //! \return the problem type, or an emptry string if not present.
  std::string type() const;

  //! \return the problem summary, or an empty string if not present.
  std::string title() const;

  //! \return the HTTP status code.
  web::http::status_code status() const;

  //! \return the problem detail.
  std::string detail() const;

  //! \return the URI reference that identifies the occurrence of this problem,
  //! or an empty string if not present.
  std::string instance() const;
};

//! \return an error string from a serialized ProblemDetails
std::string safeProblemDetailsString(const web::json::value& aJson) noexcept;

} // namespace rest
} // namespace uiiit

std::ostream& operator<<(std::ostream&                      aStream,
                         const uiiit::rest::ProblemDetails& aProblemDetails);
