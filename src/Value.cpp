#include "Value.hpp"

namespace ECE141 {
  ValueType::operator string() {
    // convert the Data to string
    if (type == DataType::float_type) {
      // TODO: null
      try {
          return to_string(std::get<float>(value));
      } catch(...) {
        return "";
      }
    } else if (type == DataType::datetime_type) {
      try {
        return std::get<string>(value);
      } catch(...) {
        return "";
      }
    } else if (type == DataType::varchar_type) {
      try{
        return std::get<string>(value);
      } catch(...) {
          return "";
      }
    } else if (type == DataType::bool_type) {
      try{
        return std::get<bool>(value) == true ? "true" : "false";
      } catch(...) {
          return "";
      }
    } else if (type == DataType::int_type) {
      try {
        return to_string(std::get<uint32_t>(value));
      } catch(...) {
        return "";
      }
    } else {
      return "no_type";
    }
  }

  bool ValueType::operator<(const ValueType& valueType) const{
    // there is no no_type
    if (type == DataType::int_type) {
      return get<uint32_t>(value) < get<uint32_t>(valueType.value);
    } else if (type == DataType::bool_type) {
      // suppose order by ture -> false
      return get<bool>(value) && (!get<bool>(valueType.value));
    } else if (type == DataType::float_type) {
      return get<float>(value) < get<float>(valueType.value);
    } else {
      // varchar & date time
      return get<string>(value) < get<string>(valueType.value);
    }
  }

  bool ValueType::operator==(const ValueType& valueType) const{
    // there is no no_type
    if (type == DataType::int_type) {
      return get<uint32_t>(value) == get<uint32_t>(valueType.value);
    } else if (type == DataType::bool_type) {
      // suppose order by ture -> false
      return get<bool>(value) == get<bool>(valueType.value);
    } else if (type == DataType::float_type) {
      return get<float>(value) == get<float>(valueType.value);
    } else {
      // varchar & date time
      return get<string>(value) == get<string>(valueType.value);
    }
  }

  ValueType ValueType::operator=(const ValueType& valueType) {
    this->type = valueType.type;
    this->value = valueType.value;
    return *this;
  }
}
