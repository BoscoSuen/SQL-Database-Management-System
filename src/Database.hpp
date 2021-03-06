//
//  Database.hpp
//  ECEDatabase
//
//  Created by rick gessner on 3/30/18.
//  Copyright © 2018 rick gessner. All rights reserved.
//

#ifndef Database_hpp
#define Database_hpp

#include <stdio.h>
#include <string>
#include <iostream>
#include <unordered_map>
#include "Storage.hpp"
#include "View.hpp"
#include "Schema.hpp"
#include "Row.hpp"
#include "Index.hpp"
#include "Filters.hpp"
#include "FolderReader.hpp"

using namespace std;

namespace ECE141 {
  class Schema;
//  class Filters;
  vector<string> split_str(string str,string pattern);
  vector<string> split_str(string str,string pattern , int cut);
  class Database{
  public:
    static Database* getDBInstance();
    
    static StatusResult dropDBInstance();
    
    static StatusResult createDB(const std::string aPath);
    
    static StatusResult dropDB(const std::string aPath);
    
    static StatusResult useDB(const std::string aPath);
    
    static StatusResult describeDB(const std::string aPath);

    ~Database();
    
    StatusResult  encode(std::ostringstream &aWriter);
    
    Storage&          getStorage() {return storage;}
    std::string&      getName() {return name;}

    StatusResult      descDB(ostream &output);

    StatusResult      loadSchema();
    StatusResult      getSchemaBlockNum(std::string aName);

    Schema            getSchema(string& aName);

    StatusResult      createTable(const Schema &aSchema) ;
    StatusResult      showTables(ostream &anOutput);
    StatusResult      dropTable(const string &aName);

    StatusResult      insert(Row& aRow, string aName);
    StatusResult      selectRow(Filters& filters, Schema& curSchema, RowCollection& rowCollection, vector<string>& properties, int limit);
    StatusResult      updateRow(Schema& curSchema, KeyValues& aKeyValues, Row& aRow);
    StatusResult      deleteRow(string aTableName);

    Index*            getIndexForTableField(string aTableName, string aFieldName);

    StatusResult      showIndexes();

  protected:
    std::string     name;
    Storage         storage;
    unordered_map<string, int> schemas;
    unordered_map<string, Index*> index_map;
    
  private:
    static  Database* activeDB;
    static  Database* loadDB(const std::string aPath);
    static  bool      checkDB(std::string aPath);

    Database(const std::string aPath, CreateNewStorage);
    Database(const std::string aPath, OpenExistingStorage);
  };
}

#endif /* Database_hpp */
