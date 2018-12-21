#include "datatype.h"

#include <stdexcept>

namespace uiiit {
namespace rest {

DataType::DataType()
    : theObject(web::json::value::object()) {
}

DataType::DataType(const web::json::value& aJson)
    : theObject() {
  if (not aJson.is_object()) {
    throw std::runtime_error("Error creating a DataType from a non-object (" +
                             std::to_string(static_cast<int>(aJson.type())) +
                             ")");
  }

  theObject = aJson;
}

DataType::~DataType() {
  // no-op
}

const web::json::value& DataType::toJson() const {
  return theObject;
}

bool DataType::operator==(const DataType& aAnother) const {
  return theObject == aAnother.theObject;
}

} // namespace rest
} // namespace uiiit
