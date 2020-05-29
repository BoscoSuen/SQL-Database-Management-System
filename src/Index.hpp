//
// Created by Suen on 5/15/20.
//

#ifndef Index_hpp
#define Index_hpp

#include <stdio.h>
#include <string>
#include <vector>
#include "Value.hpp"

namespace ECE141{
  class Index {
  public:
    Index(const std::string &aField, string aTableName, DataType aKeyType)
            : field(aField), tableName(aTableName), keyType(aKeyType) {}

    Index&   addKeyValue(const ValueType &aKey, uint32_t aValue);
    Index&   removeKeyValue(const ValueType &aKey);
    bool     contains(const ValueType &aKey);
    uint32_t getValue(const ValueType &aKey);
    std::string getField() {return field;}

    using IndexType = std::map<ValueType, uint32_t>; //c++ typedef...
    IndexType getIndex();

  protected:
    IndexType   index;    // the mapping of value to blockNum
    std::string field;    //name of the field being indexed
    DataType    keyType;  //what Value datatype are we using for this field?
    string      tableName;
  };
}




#endif //Index_hpp
