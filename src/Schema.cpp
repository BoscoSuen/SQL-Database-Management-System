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

  Schema::Schema(const std::string aName) : name(aName), attributes(), changed(false), blockNum(0) , hasPrimaryKeyName(false) , primaryKeyName("") , AutoIncreasing(false) , prevValue(-1){}

Schema::Schema(const Schema &aCopy) : name(aCopy.name), attributes(aCopy.attributes), changed(aCopy.changed), blockNum(aCopy.blockNum) , hasPrimaryKeyName(aCopy.hasPrimaryKeyName) , primaryKeyName(aCopy.primaryKeyName) , AutoIncreasing(aCopy.AutoIncreasing) , prevValue(-1){}

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
    return Attribute();
  }

  StatusResult Schema::encode(std::ostringstream &aWriter) const{
   aWriter <<
              name << "|" <<
              blockNum << "|" <<
              (changed == true ? "1" : "0") << "#";
   return StatusResult{Errors::noError};
  }

  StatusResult Schema::decode(Schema& schema, std::string str) {
    std::vector<std::string> arr = split_str(str, "|");
    std::string name = arr[0];
    Schema ret = Schema(name);
    uint32_t blockNumber = (uint32_t)(std::stoi(arr[1]));
    ret.setBlockNum(blockNumber);
    std::vector<std::string> sub = split_str(arr[2], "#");
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
}
