#include "Value.hpp"

namespace ECE141 {
  ValueType::operator string() {
    // convert the Data to string
    if (type == DataType::float_type) {
      // TODO: null
      return to_string(std::get<float>(value));
    } else if (type == DataType::datetime_type) {
      return std::get<string>(value);
    } else if (type == DataType::varchar_type) {
      return std::get<string>(value);
    } else if (type == DataType::bool_type) {
      return std::get<bool>(value) == true ? "true" : "false";
    } else if (type == DataType::int_type) {
      return to_string(std::get<uint32_t>(value));
    } else {
      return "no_type";
    }
  }
}