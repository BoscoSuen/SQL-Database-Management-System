//
//  Value.hpp
//  RGAssignment5
//
//  Created by rick gessner on 4/26/20.
//  Copyright © 2020 rick gessner. All rights reserved.
//

#ifndef Value_h
#define Value_h

#include <variant>
#include <optional>
#include <string>
#include <map>
#include <cstdint>
#include <unordered_map>
#include "Errors.hpp"

using namespace std;

namespace ECE141 {

  struct Storable {
    virtual StatusResult  encode(std::ostringstream &aWriter) const =0;
//     virtual StatusResult  decode(std::istream &aReader)=0;
  };

  enum class DataType {
     no_type='N', bool_type='B', datetime_type='D', float_type='F', int_type='I', varchar_type='V',
   };

  using Value = std::variant<uint32_t, float, bool, std::string>;

  class ValueType {
  public:
    ValueType(){}
    ValueType(Value aValue , DataType aType) : value(aValue) , type(aType) {}
    ValueType(uint32_t aValue , DataType aType) : value(aValue) , type(aType) {}
    ValueType(float aValue , DataType aType) : value(aValue) , type(aType) {}
    ValueType(bool aValue , DataType aType) : value(aValue) , type(aType) {}
    ValueType(std::string aValue , DataType aType) : value(aValue) , type(aType) {}
    
    ValueType(const ValueType& another) {
      this->value = another.value;
      this->type = another.type;
    }

    operator string();


    Value value;
    DataType type;
  };

  using KeyValues = std::map<const std::string, ValueType>;

  using KeyValue = std::pair<const std::string, ValueType>;

  static std::unordered_map<char, std::string> BlockTypeStrings{
          {'T', "Meta"},
          {'D', "Data"},
          {'E', "Entity"},
          {'F', "Free"},
          {'I', "Index"},
          {'V', "Unknow"},
  };

  static std::unordered_map<DataType, string> DataTypeToStr{
          {DataType::varchar_type, "V"},
          {DataType::int_type, "I"},
          {DataType::float_type, "F"},
          {DataType::bool_type, "B"},
          {DataType::datetime_type, "D"},
          {DataType::no_type, "N"}
  };

  static std::unordered_map<string, DataType> StrToDataType{
          {"V", DataType::varchar_type},
          {"I", DataType::int_type},
          {"F", DataType::float_type},
          {"B", DataType::bool_type},
          {"D", DataType::datetime_type},
          {"N", DataType::no_type}
  };
}

#endif /* Value_h */
