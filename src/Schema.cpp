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

  Schema::Schema(const std::string aName) : name(aName), attributes(), changed(false), blockNum(0) , primaryKeyName("") , AutoIncreasing(false) , prevValue(0){}

Schema::Schema(const Schema &aCopy) : name(aCopy.name),  changed(aCopy.changed), blockNum(aCopy.blockNum), primaryKeyName(aCopy.primaryKeyName) , AutoIncreasing(aCopy.AutoIncreasing) , prevValue(aCopy.prevValue){
    for (Attribute att : aCopy.attributes) {
      attributes.push_back(att);
    }
//    attributes(aCopy.attributes);
  }

  Schema::~Schema(){}



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

  bool Schema::attrInSchema(string attrName) {
    AttributeList attributeList = this->getAttributes();
    for (auto it = attributeList.begin(); it != attributeList.end(); ++it) {
      if (it->getName() == attrName) return true;
    }
    return false;
  }

  StatusResult Schema::encodeKeyValues(StorageBlock& aBlock, KeyValues& data) {
    // get first pos after write the table name:
    int pos = 0;
    while (pos < kPayloadSize && aBlock.data[pos] != '\0') { pos++; }
    for (auto attr : attributes) {
      stringstream ss;
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

  Value getData(string str) {
    if (str.find("true") != -1) return true;
    else if(str.find("false") != -1) return false;
    for (int i = 0; i < str.size(); ++i) {
      if (!isdigit(str[i]) && str[i] != '.') return str;
      else if (str[i] == '.') {
        // float
        istringstream is(str);
        float res;
        is >> res;
        return res;
      }
    }
    istringstream is(str);
    uint32_t res;
    is >> res;
    return res;
  }

  StatusResult Schema::decodeWithProperties(StorageBlock& curBlock, vector<string>& properties, KeyValues& keyValues) {
    // @ << name << "|" << DataTypeToStr[valueType.type] << "|" << data << "*"
    vector<string> strs = split_str(curBlock.data, "@" , 1);
    string dataList = strs[1];
    vector<string> pairs = split_str(dataList, "*");
    for (string pair : pairs) {
      if (pair.size() == 0) continue;
      vector<string> kvpairs = split_str(pair, "|");
      // name|type|data   KeyValues<string, ValueType<value, data>>
      string name(kvpairs[0]);
      DataType type(StrToDataType.at(kvpairs[1]));
      Value data(getData(kvpairs[2]));
      ValueType valueType(data, type);
//      if (properties.empty() || (find(properties.begin(), properties.end(), name) != properties.end())) {
      keyValues[name] = valueType;
//      }
    }
    return StatusResult{Errors::noError};
  }


  std::string Schema::getPrimaryKeyName() const {
    return primaryKeyName.size() ? primaryKeyName : "id";
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

  DataType Schema::getPrimaryKeyType() const {
    for (auto& attr: attributes) {
      if (attr.getPrimary()) {
        return attr.getType();
      }
    }
    return DataType::int_type;  // set default
  }
}
