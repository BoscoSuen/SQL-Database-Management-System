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
      for (int i = (int)(str.size()) ; i < leng - 1; i ++) {
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
    int rowsCount = 0;
    StorageBlock curBlock;
    for (uint32_t i = 0; i < storage.getTotalBlockCount(); ++i) {
      StatusResult readRes = storage.readBlock(curBlock, i);
      if (!readRes) {return false;}
      rowsCount ++;
      switch (curBlock.header.type) {
        case 'T':{
          std::vector<std::string> tmp = {to_string(i) , BlockTypeStrings[curBlock.header.type] , ""};
          printRow(tmp);
          break;
        }
        case 'D':{
          std::vector<std::string> array = split_str(curBlock.data, "@" , 1);
          std::vector<std::string> tmp = {to_string(i) , "data" , "\""+ array[0] + "\""};
          printRow(tmp);
          break;
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

  ShowTableView::ShowTableView(Storage& aStorage) : storage(aStorage) , stream(nullptr){
    length = {10};
    total = 0; for (int i : length) total += i;
  }

  void printARow(std::ostream &aStream , int length) {
    aStream << "+" ;
    for (int i = 0 ; i < length ; i++ )
      aStream << "-" ;
    aStream << "+" << std::endl ;
  }

  bool ShowTableView::show(std::ostream &aStream) {
    bool status = true;
    std::vector<std::string> head = {Database::getDBInstance()->getName()};
    
    printRowSeparator();
    printRow(head);
    printRowSeparator();
    
    int rows = 0;
    StorageBlock curBlock;
    for (uint32_t i = 0; i < storage.getTotalBlockCount(); ++i) {
      StatusResult readRes = storage.readBlock(curBlock, i);
      if (readRes && curBlock.header.type == 'E') {
        std::vector<std::string> name =
                      {split_str(curBlock.data, "|")[0]};
        printRow(name);
        rows++;
      }
    }
    printRowSeparator();
    aStream << rows << " rows in set" << std::endl;
    return status;
  }

  DescTableView::DescTableView(Schema& aSchema) : schema(aSchema) , stream(nullptr) {
    length = {11 , 12 , 6 , 5 , 9 , 28};
    total = 0; for (int i : length) total += i;
  }

  bool DescTableView::show(std::ostream& aStream) {
    int rows = 0;
    std::vector<std::string> head = {"Field" , "Type" , "Null" , "Key" , "Default" , "Extra"};
    printRowSeparator();
    printRow(head);
    printRowSeparator();

    AttributeList attrList = schema.getAttributes();
    for (auto attr : attrList) {
      rows ++;
      std::stringstream tmp;
      tmp << (attr.getAutoIncreasing() ? "auto_increment " : "")
          << (attr.getPrimary() ? "primary key" : "");
      std::vector<std::string> strList = {
        attr.getName() ,
        attr.getTypeString() ,
        attr.getNullable() ? "YES" : "NO" ,
        attr.getPrimary() ? "YES" : "" ,
        attr.getHasDefault() ? attr.getDefaultValue() : "NULL" ,
        tmp.str()
      };
      printRow(strList);
    }
    printRowSeparator();
    aStream << rows << " rows in set" << std::endl;
    return true;
  }

  SelectTableView::SelectTableView(AttributeList& attrList , std::vector<Row*> rowList) : attributes(attrList) , rows(rowList){
    static unordered_map<DataType, int> defaultLengthMap = {
      {DataType::no_type ,       10} ,
      {DataType::bool_type ,     7 } ,
      {DataType::datetime_type , 12} ,
      {DataType::float_type ,    7 } ,
      {DataType::int_type   ,    6 } ,
      {DataType::varchar_type ,  24 }
    };
    for (Attribute& att : attrList){
      head.push_back(att.getName());
      int leng =
          std::max( (int)(att.getName().size()) + 2 ,
                    defaultLengthMap.find(att.getType())->second  );
//      if (att.getType() == DataType::varchar_type)
//        leng = std::max(leng , stoi(att.getVarCharOther()));
      length.push_back(leng);
    }
    total = 0; for (int i : length) total += i;
  }

  bool SelectTableView::show(std::ostream& aStream){
    printRowSeparator();
    printRow(head);
    printRowSeparator();
    
    for (Row* row : rows) {
      KeyValues& data = row->getData();
      std::vector<std::string> array;
      for (std::string& attr : head){
        array.push_back(data.find(attr)->second);
      }
      printRow(array);
    }
    printRowSeparator();
    aStream << rows.size() << " rows in set";
//    << std::endl;
    return true;
  }

  ShowIndexView::ShowIndexView(std::vector<std::vector<std::string>>& data) : list(data) {
    length.push_back(12);
    length.push_back(12);
    total = 24;
  }

  bool ShowIndexView::show(std::ostream& aStream){
    printRowSeparator();
    std::vector<std::string> head = {"table" , "field"};
    printRow(head);
    printRowSeparator();
    int index = 0;
    while (index < list.size()) {
      printRow(list[index]);
      index++;
    }
    printRowSeparator();
    aStream << index << " rows in set" << std::endl;
    return true;
  }
}

