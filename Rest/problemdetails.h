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
