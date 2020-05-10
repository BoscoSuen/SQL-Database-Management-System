//
//  View.cpp
//  Assignment3
//
//  Created by Hongxiang Jiang and Zhiqiang Sun on 2020/4/20.
//  Copyright © 2020 Hongxiang Jiang and Zhiqiang Sun. All rights reserved.
//

#include "View.hpp"
#include <stdio.h>
#include <unordered_map>
#include <cstdio>

namespace ECE141 {
    DescDBView::DescDBView(Storage& aStorage) : stream(nullptr), storage(aStorage) {}

    bool DescDBView::show(std::ostream &aStream) {
      static std::unordered_map<char, std::string> BlockTypeStrings{
              {'T', "Meta"},
              {'D', "Data"},
              {'E', "Entity"},
              {'F', "Free"},
              {'I', "Index"},
              {'V', "Unknow"},
      };
      bool status = true;
      try {
        StorageBlock curBlock;
        aStream << "Blk#  Type    Other" << std::endl;
        aStream << "----------------------------" << std::endl;
        for (uint32_t i = 0; i < storage.getTotalBlockCount(); ++i) {
          StatusResult readRes = storage.readBlock(curBlock, i);
          if (readRes) {
            if (curBlock.header.type == 'F') {
              continue;
            } else {
              aStream << i << " " << BlockTypeStrings[curBlock.header.type] << std::endl;
            }
          }
        }
      } catch(...) {
        status = false;
      }
      return status;
    }

    ShowTableView::ShowTableView(Storage& aStorage) : storage(aStorage) , stream(nullptr){};

    void printRow(std::ostream &aStream , int length) {
      aStream << "+" ;
      for (int i = 0 ; i < length ; i++ )
        aStream << "-" ;
      aStream << "+" << std::endl ;
    }

    bool ShowTableView::show(std::ostream &aStream) {
      bool status = true;
      int rows = 0;
      try {
        StorageBlock curBlock;
        int maxLength = Database::getDBInstance()->getName().length();
        
        for (uint32_t i = 0; i < storage.getTotalBlockCount(); ++i) {
          StatusResult readRes = storage.readBlock(curBlock, i);
          if (readRes && curBlock.header.type == 'E') {
//            std::clog << curBlock.data << std::endl;
            char* tmp = curBlock.data;
            rows ++;
            for (int i = 0 ; i < sizeof(curBlock.data) / sizeof(tmp[0]) ; i++) {
              if (tmp[i] == '|') {
                maxLength = std::max(i , maxLength);break;
              }
            }
          }
        }

        int width = maxLength + 2;
        printRow(aStream , width);
        aStream << "| " << Database::getDBInstance()->getName();
        for (int j = Database::getDBInstance()->getName().length() ; j < maxLength ; j++) aStream << " " ;
        aStream << " |" << std::endl;
        printRow(aStream , width);
        for (uint32_t i = 0; i < storage.getTotalBlockCount(); ++i) {
          StatusResult readRes = storage.readBlock(curBlock, i);
          if (readRes && curBlock.header.type == 'E') {
            aStream << "| " ;
            int already = 0;
            char* tmp = curBlock.data;
            for (int i = 0 ; i < sizeof(curBlock.data) / sizeof(tmp[0]) ; i++){
              if (tmp[i] != '|') {aStream << tmp[i]; already ++;}
              else break;
            }
            for (int j = already ; j < maxLength ; j++) aStream << " " ;
            aStream << " |" << std::endl;
          }
        }
        printRow(aStream , width);
      } catch(...) {
        status = false;
      }
      aStream << rows << " rows in set" << std::endl;
      return status;
    }

  DescTableView::DescTableView(Schema& aSchema) : schema(aSchema) , stream(nullptr) {};

  bool DescTableView::show(std::ostream& aStream) {
    int rows = 0;
    const char* tableSep = "+-----------+--------------+------+-----+---------+-----------------------------+";
    const char* title    = "| Field     | Type         | Null | Key | Default | Extra                       |";
    aStream << tableSep << "\n" << title << "\n" << tableSep << endl;
    int lenField = 10,lenType = 13,lenNull = 5,lenKey = 4,lenDefault = 8,lenExtra = 28;

    AttributeList attrList = schema.getAttributes();
    for (auto attr : attrList) {
      rows ++;
      aStream << "| " << attr.getName();
      for (int i = attr.getName().size(); i < lenField; ++i) aStream << " ";
      aStream << "| " << attr.getTypeString();
      for (int i = attr.getTypeString().size(); i < lenType; ++i) aStream << " ";
      string nullable = attr.getNullable() ? "YES" : "NO";
      aStream << "| " << nullable;
      for (int i = nullable.size(); i < lenNull; ++i) aStream << " ";
      string isPrimaryKey = attr.getPrimary() ? "YES" : "";
      aStream << "| " << isPrimaryKey;
      for (int i = isPrimaryKey.size(); i < lenKey; ++i) aStream << " ";
      string defaultVal = attr.getHasDefault() ? attr.getDefaultValue() : "NULL";
      aStream << "| " << defaultVal;
      for (int i = defaultVal.size(); i < lenDefault; ++i) aStream << " ";
      string extra = attr.getAutoIncreasing() ? "auto_increment" : "";
      extra += !isPrimaryKey.empty() ? "primary key" : "";
      aStream << "| " << extra;
      for(int i = extra.size(); i < lenExtra; ++i) aStream << " ";
      aStream << "|" << endl;
    }
    aStream << tableSep << endl;
    aStream << rows << " rows in set" << std::endl;
    return true;
  }
}
