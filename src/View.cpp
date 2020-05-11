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
  static const std::string CornerSep = "+";
  static const std::string ColumnSep = "|";
  static const std::string RowSep = "-";

  void View::printFormatedData(std::string str, int leng) {
      std::cout << " ";
      std::cout << str;
      for (int i = str.size() ; i < leng - 1; i ++) {
        std::cout << " ";
      }
  }

  void View::printRowSeparator() {
    std::cout << CornerSep;
    for (int j = 0 ; j < length.size() ; j++) {
      int leng = length[j];
      for (int i = 0 ; i < leng ; i++) {
        std::cout << RowSep;
      }
      std::cout << CornerSep;
    }
    std::cout << std::endl;
  }

  void View::printRow(std::vector<std::string> array) {
    std::cout << ColumnSep;
    for (int i = 0 ; i < array.size() ; i++) {
      printFormatedData(array[i] , length[i]);
      std::cout << ColumnSep;
    }
    std::cout << std::endl;
  }

  DescDBView::DescDBView(Storage& aStorage) : stream(nullptr), storage(aStorage) {
    length = {6 , 13 , 25};
    total = 0; for (int i : length) total += i;
  }

  bool DescDBView::show(std::ostream &aStream) {
    printRowSeparator();
    std::vector<std::string> head = {"Blk#" , "Type" , "Other"};
    printRow( head );
    printRowSeparator();
//    aStream << "Blk#  Type    Other" << std::endl;
//    aStream << "----------------------------" << std::endl;
    int rowsCount = 0;
    StorageBlock curBlock;
    for (uint32_t i = 0; i < storage.getTotalBlockCount(); ++i) {
      StatusResult readRes = storage.readBlock(curBlock, i);
//      std::cout << curBlock.header.type << curBlock.data << std::endl;
      if (!readRes) {return false;}
      rowsCount ++;
      switch (curBlock.header.type) {
        case 'T':{
          std::vector<std::string> tmp = {to_string(i) , BlockTypeStrings[curBlock.header.type] , ""};
          printRow(tmp);
          break;
        }
        case 'D':{
          std::vector<std::string> array = split_str(curBlock.data, "@");
          std::vector<std::string> tmp = {to_string(i) , "data" , "\""+ array[0] + "\""};
          printRow(tmp);
          break;
        }
        case 'S': {
          
        }
        case 'E':{
          Schema schema("");
          Schema::decode(schema , curBlock.data);
          std::vector<std::string> tmp = {to_string(i) , "schema" , "\""+ schema.getName() + "\""};
          printRow(tmp);
          break;
        }
        default:{
          rowsCount--;
          break;
        }
      }
    }
    printRowSeparator();
    aStream << to_string(rowsCount) << " rows in set" << std::endl;
    return true;
  }

  ShowTableView::ShowTableView(Storage& aStorage) : storage(aStorage) , stream(nullptr){};

  void printARow(std::ostream &aStream , int length) {
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
        printARow(aStream , width);
        aStream << "| " << Database::getDBInstance()->getName();
        for (int j = Database::getDBInstance()->getName().length() ; j < maxLength ; j++) aStream << " " ;
        aStream << " |" << std::endl;
        printARow(aStream , width);
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
        printARow(aStream , width);
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
      extra += !isPrimaryKey.empty() ? " primary key" : "";
      aStream << "| " << extra;
      for(int i = extra.size(); i < lenExtra; ++i) aStream << " ";
      aStream << "|" << endl;
    }
    aStream << tableSep << endl;
    aStream << rows << " rows in set" << std::endl;
    return true;
  }
}
