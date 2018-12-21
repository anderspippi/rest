#pragma once

#include <cpprest/json.h>

namespace uiiit {
namespace rest {

/**
 * Generic REST data type.
 */
class DataType
{
 public:
  virtual ~DataType();

  //! \return the JSON representation of this object.
  const web::json::value& toJson() const;

  //! \return true if the two objects have the same content.
  bool operator==(const DataType& aAnother) const;

 protected:
  explicit DataType();

  /**
   * Data type from JSON.
   *
   * \throws std::runtime_error if aJson is not an object.
   */
  explicit DataType(const web::json::value& aJson);

 protected:
  web::json::value theObject;
};

} // namespace rest
} // namespace uiiit
