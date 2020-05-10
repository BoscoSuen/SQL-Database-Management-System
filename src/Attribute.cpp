//
//  Attribute.cpp
//  Assignment4
//
//  Created by rick gessner on 4/18/20.
//  Copyright © 2020 rick gessner. All rights reserved.
//

#include "Attribute.hpp"

namespace ECE141 {
    //STUDENT: Implement the attribute class here...

    Attribute::Attribute(DataType aType) : type(aType) , autoIncrease(false) , primary(false) , nullable(true) , hasDefault(false) , defaultValue("") , varCharOther(""){};

    Attribute::Attribute(std::string aName, DataType aType) : name(aName),type(aType) , autoIncrease(false) , primary(false) , nullable(true) , hasDefault(false) , defaultValue("") , varCharOther("") {};

    Attribute::Attribute(const Attribute &aCopy) {
      this->name = aCopy.getName();
      this->type = aCopy.getType();
      this->autoIncrease = aCopy.getAutoIncreasing();
      this->primary = aCopy.getPrimary();
      this->nullable = aCopy.getNullable();
      this->hasDefault = aCopy.getHasDefault();
      this->defaultValue = aCopy.getDefaultValue();
      this->varCharOther = aCopy.getVarCharOther();
    };

    Attribute::~Attribute(){};

    bool Attribute::setDefaultValue(std::string a) {
      defaultValue = a;
      return true;
    }

    string Attribute::getTypeString() {
      unordered_map<DataType,std::string> tableTypeMap = {
              {DataType::no_type, "no_type"},
              {DataType::int_type, "integer"},
              {DataType::float_type, "float"},
              {DataType::bool_type, "bool"},
              {DataType::varchar_type, "varchar"},
              {DataType::datetime_type, "date"}
      };
      return tableTypeMap.at(getType());
    }


    StatusResult  Attribute::encode(std::ostringstream &aWriter) const{
      unordered_map<DataType, char> map = {
              {DataType::no_type, 'N'},{DataType::bool_type, 'B'},{DataType::float_type, 'F'}, {DataType::datetime_type, 'D'},
              {DataType::int_type, 'I'},{DataType::varchar_type, 'V'}
      };
      aWriter <<
        name << "*" <<
        map.at(type) << "*"<<
        (getAutoIncreasing() ? "1" : "0" ) << "*" <<
        (getPrimary() ? "1" : "0") << "*" <<
        (getNullable() ? "1" : "0") << "*" <<
        (getHasDefault() ? "1" : "0") << "*" <<
        getDefaultValue() << "*" <<
        getVarCharOther() <<
        "$";
      return StatusResult();
    }
//CREATE TABLE table3 ( attr2 FLOAT DEFAULT 0.0 )
  StatusResult Attribute::decode(Attribute& att, std::string str) {
    unordered_map<char,DataType> reverseMap = {
            {'N',DataType::no_type},
            {'B',DataType::bool_type},
            {'F',DataType::float_type},
            {'D',DataType::datetime_type},
            {'I',DataType::int_type},
            {'V',DataType::varchar_type}
    };
    vector<std::string> total = ECE141::split_str(str, "*");
    if (total.size() < 8) return StatusResult(Errors::decodeError , 0);
    std::string name = total[0];
    std::string t = total[1];
    if (t.size() != 1) return StatusResult(Errors::seekError , 0);
    auto iter = reverseMap.find(t[0]);
    if (iter == reverseMap.end()) return StatusResult(Errors::decodeError , 0);
    DataType type = iter->second;
    
    Attribute ret = Attribute(name , type);
    ret.setAutoIncreasing((total[2].compare("0") == 0 ? false : true));
    ret.setPrimary((total[3].compare("0") == 0 ? false : true));
    ret.setNullable((total[4].compare("0") == 0 ? false : true));
    ret.setHasDefault((total[5].compare("0") == 0 ? false : true));
    
    ret.setDefaultValue(total[6]);
    ret.setVarCharOther(total[7]);
    
    att = ret;
    return StatusResult();
  }
}
