//
//  Schema.cpp
//  Assignment4
//
//  Created by rick gessner on 4/18/20.
//  Copyright © 2020 rick gessner. All rights reserved.
//

#include "Schema.hpp"

namespace ECE141 {

 //STUDENT: Implement the Schema class here...

  Schema::Schema(const std::string aName) : name(aName), attributes(), changed(false), blockNum(0) , hasPrimaryKeyName(false) , primaryKeyName("") , AutoIncreasing(false) , prevValue(0){}

  Schema::Schema(const Schema &aCopy) : name(aCopy.name),  changed(aCopy.changed), blockNum(aCopy.blockNum) , hasPrimaryKeyName(aCopy.hasPrimaryKeyName) , primaryKeyName(aCopy.primaryKeyName) , AutoIncreasing(aCopy.AutoIncreasing) , prevValue(0){
    for (Attribute att : aCopy.attributes) {
      attributes.push_back(att);
    }
//    attributes(aCopy.attributes);
  }

  Schema::~Schema(){};

  Schema& Schema::addAttribute(const Attribute &anAttribute) {
   attributes.push_back(anAttribute);
   return *this;
  }

  const Attribute& Schema::getAttribute(const std::string &aName) const {
    try {
      for (auto it = attributes.begin(); it != attributes.end(); ++it) {
        if (it->getName() == aName) {
          return *it;
        }
      }
    } catch(...) {
      std::clog << "There is no such attribute!\n";
    }
    // just for compile
    return *(new Attribute(DataType::no_type));
  }

  StatusResult Schema::encode(std::ostringstream &aWriter) const{
    aWriter <<
              name << "|" <<
              blockNum << "|" <<
              primaryKeyName << "|" <<
              (AutoIncreasing ? "1" : "0") << "|" <<
              prevValue << "|" <<
              (changed == true ? "1" : "0") << "|" <<
              "#";
    return StatusResult{Errors::noError};
  }

  StatusResult Schema::encodeKeyValues(StorageBlock& aBlock, KeyValues& data) {
    // get first pos after write the table name:
    int pos = 0;
    while (pos < kPayloadSize && aBlock.data[pos] != '\0') { pos++; }
    stringstream ss;
    for (auto attr : attributes) {
      if (data.count(attr.getName())) {
        string name = attr.getName();
        ValueType valueType = data.at(name);
        string data = valueType;
        ss << name << "|" << DataTypeToStr[valueType.type] << "|" << data << "*";
      }
      string out = ss.str();
      if (pos + out.size() >= kPayloadSize) return StatusResult{Errors::storageFull};
      strcpy(aBlock.data + pos, out.c_str());
      pos += out.size();
    }
    return StatusResult{Errors::noError};
  }

  StatusResult Schema::decode(Schema& schema, std::string str) {
    std::vector<std::string> arr = split_str(str, "|");
    std::string name = arr[0];
    Schema ret = Schema(name);
    uint32_t blockNumber = (uint32_t)(std::stoi(arr[1]));
    ret.setBlockNum(blockNumber);
    std::string cur = arr[2];
    ret.setPrimaryKeyName(cur);
    cur = arr[3];
    ret.setAutoIncreasing(cur.compare("1") == 0 ? true : false);
    cur = arr[4];
    ret.setPrevValue(std::stoi(cur));
    std::vector<std::string> sub = split_str(arr[arr.size() - 1], "#");
    std::string change = sub[0];
    ret.setChanged((change.compare("1") == 0) ? true : false);
    std::vector<std::string> attributesArr = split_str(sub[1],"$");
    for (std::string attributeString : attributesArr) {
      if (attributeString.size() == 0) continue;
      Attribute tmp;
      StatusResult attDecodeRes = Attribute::decode(tmp, attributeString);
      if (!attDecodeRes) return attDecodeRes;
      ret.addAttribute(tmp);
    }
    schema = ret;
    return StatusResult();
  }

  std::string Schema::getPrimaryKeyName() const {
    if (hasPrimaryKeyName) return primaryKeyName;
    else return "";
  }

  bool Schema::validRow(KeyValues data) {
//    for (auto it = data.begin(); it != data.end(); ++it) {
//      for (auto attr : attributes) {
//        if (it->second.type != attr.getType() || it->first != attr.getName()) {
//          return false;
//        }
//      }
//    }
    return true;
  }
}
