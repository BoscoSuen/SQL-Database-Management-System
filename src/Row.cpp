//
//  Row.cpp
//  Assignment4
//
//  Created by rick gessner on 4/19/20.
//  Copyright © 2020 rick gessner. All rights reserved.
//

#include "Row.hpp"

namespace ECE141 {

    //STUDENT: You need to fully implement these methods...

    Row::Row(int32_t blockNum) : blockNum(blockNum) , data() {}
    Row::Row(const Row &aRow) {
      data = aRow.copyData();
      blockNum = aRow.blockNum;
    }
    Row::Row(KeyValues& keyValues, uint32_t blockNum) {
      this->blockNum = blockNum;
      for (auto it = keyValues.begin(); it != keyValues.end(); ++it) {
        this->data[it->first] = it->second;
      }
    }
    Row& Row::operator=(const Row &aRow) {
//    data = aRow.data;
//    blockNum = aRow.blockNum;
      this->blockNum = aRow.blockNum;
      for (auto it = aRow.data.begin(); it != aRow.data.end(); ++it) {
        this->data[it->first] = it->second;
      }
      return *this;
    }
    bool Row::operator==(Row &aCopy) const {
      KeyValues keyValues = aCopy.getData();
      if (data.size() != keyValues.size()) return false;
      for (auto it = data.begin(); it != data.end(); ++it) {
        if (!(it->second.value == keyValues.at(it->first).value && it->second.type == keyValues.at(it->first).type)) return false;
      }
      return false;
    }
    Row::~Row() {}

    void Row::setBlockNum(uint32_t blockNum) {
      this->blockNum = blockNum;
    }

    uint32_t Row::getBlockNum() {
      return blockNum;
    }


    StatusResult Row::encode(std::ostringstream &aWriter) const {
      // to be written
      return StatusResult();
    }
    vector<string> Row::split(string str,string pattern) {
      string::size_type pos;
      vector<string> result;
      str += pattern;
      size_t size=str.size();
      for(size_t i = 0; i < size; i++){
        pos = str.find(pattern, i);
        if(pos < size){
          string s = str.substr(i,pos - i);
          result.push_back(s);
          i = pos+pattern.size()-1;
        }
      }
      return result;
    }

    Value Row::getData(string str) {
      if (str.find("true") != -1) return true;
      else if(str.find("false") != -1) return false;
      for (int i = 0; i < (int)(str.size()); ++i) {
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

    StatusResult Row::decode(Row& row, std::string str) {
      // to be written
//    Row aRow;   // default blockNum 0, do not need
      vector<string> pairs = split(str, "*");
      for (auto pair : pairs) {
        if (pair.size() == 0) continue;
        vector<string> strs = split(pair, "|");
        // name | type_str | data
        string name(strs[0]);
        DataType type(StrToDataType.at(strs[1]));
        Value data(getData(strs[2]));
        ValueType valueType(data, type);
        row.data[name] = valueType;
      }
//    row = aRow;
      return StatusResult{Errors::noError};
    }

    RowCollection::~RowCollection() {
      for (auto row : rowList) {
        delete row;
      }
    }

    vector<Row*> RowCollection::getRows() {
      return rowList;
    }

    RowCollection& RowCollection::addRow(Row* aRow) {
      rowList.push_back(aRow);
      return *this;
    }


    RowCollection& RowCollection::order(Schema& aSchema, string& orderBy) {
      sort(rowList.begin(), rowList.end(), [orderBy](Row* r1, Row* r2) {
          ValueType v1 = r1->getData()[orderBy];
          ValueType v2 = r2->getData()[orderBy];
          return v1 < v2;
      });
      return *this;
    }

    RowCollection& RowCollection::remainLimitPart(int limit) {
      while (rowList.size() > limit) {
        rowList.pop_back();
      }
      return *this;
    }
}
