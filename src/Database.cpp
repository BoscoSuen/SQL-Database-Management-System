//
//  Database.cpp
//  Database1
//
//  Created by rick gessner on 4/12/19.
//  Copyright © 2019 rick gessner. All rights reserved.
//

#include <sstream>
#include "Database.hpp"
#include "View.hpp"
#include "Storage.hpp"

//this class represents the database object.
//This class should do actual database related work,
//we called upon by your db processor or commands

namespace ECE141 {
    Database* Database::activeDB = nullptr;

    Database* Database::getDBInstance() {
      return activeDB;
    }

    StatusResult Database::dropDBInstance() {
      if (activeDB) {
        // may need to flush out some modification to storage
        delete activeDB;
        activeDB = nullptr;
      }
      return StatusResult();
    }

    StatusResult Database::createDB(const std::string aPath) {
      if (checkDB(aPath))
        return StatusResult(Errors::databaseExists , 0);
      Database* newPointer = new Database(aPath , CreateNewStorage{});
      delete newPointer;
      return StatusResult();
    }

    StatusResult Database::dropDB(const std::string aPath) {
      FolderReader *folderReader = new FolderReader(StorageInfo::getDefaultStoragePath());
      if (!folderReader->exists(Storage::getDBPath(aPath))) {
        return StatusResult(Errors::unknownDatabase, 0);
      }
      ostringstream out;
      out << StorageInfo::getDefaultStoragePath() << "/" << aPath << ".db";
      filesystem::remove(out.str());
      return StatusResult{noError};
    }

    StatusResult Database::useDB(const std::string aPath) {
      if (!checkDB(aPath))
        return StatusResult(Errors::unknownDatabase , 0);
      Database* pointer = new Database(aPath , OpenExistingStorage{});
      if (pointer) {
        delete activeDB;
        activeDB = pointer;
        return StatusResult();
      } else {
        return StatusResult(Errors::unknownError , 0 );
      }
    }

    StatusResult Database::describeDB(std::string aPath) {
      if (!checkDB(aPath))
        return StatusResult(Errors::unknownDatabase , 0);
      return loadDB(aPath)->descDB(std::cout);
    }

    Database* Database::loadDB(const std::string aPath) {
      if (activeDB && activeDB->getName().compare(aPath))
        return activeDB;
      Database* newPointer = new Database(aPath , OpenExistingStorage{});
      if (!newPointer) {
        return nullptr;
      } else {
        // may need to flush out some modification to storage
        delete activeDB;
        activeDB = newPointer;
        return activeDB;
      }
    }

    bool Database::checkDB(const std::string aPath) {
      FolderReader folderReader(StorageInfo::getDefaultStoragePath());
      if (folderReader.exists(Storage::getDBPath(aPath)))
        return true;
      else
        return false;
    }

    Database::Database(const std::string aName, CreateNewStorage)
            : name(aName), storage(aName, CreateNewStorage{}) {
    }

    Database::Database(const std::string aName, OpenExistingStorage)
            : name(aName), storage(aName, OpenExistingStorage{}) {
    }

    Database::~Database() {
    }

    StatusResult  encode(std::ostringstream &aWriter){
      return StatusResult();
    }

    StatusResult Database::descDB(ostream &output) {
      StatusResult res{Errors::noError};
      try {
        View* view = new DescDBView(storage);
        view->show(output);
        delete view;
      } catch(...) {
        res.code = Errors::unknownError;
      }
      return res;
    }

    StatusResult Database::encode(std::ostringstream& aWriter) {
      return StatusResult();
    }

    vector<string> split_str(string str,string pattern) {
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

    StatusResult Database::loadSchema() {
      StorageBlock curBlock;
      for (uint32_t i = 0; i < storage.getTotalBlockCount(); ++i) {
        StatusResult readRes = storage.readBlock(curBlock, i);
        if (readRes) {
          if (curBlock.header.type == 'E') {
            vector<string> titles = split_str(curBlock.data,"|");
            if(schemas.count(titles[0])) continue;
            schemas[titles[0]] = i;
          }
        }
      }
      return StatusResult{noError};
    }

  StatusResult Database::getSchemaBlockNum(std::string aName) {
    auto res = schemas.find(aName);
    if (res == schemas.end()) {
      // not found
      loadSchema();
      res = schemas.find(aName);
      if (res == schemas.end()) return StatusResult(Errors::unknownTable , 0);
    }
    return StatusResult(Errors::noError , res->second);
  }

    StatusResult Database::createTable(const Schema &aSchema) {
      StatusResult loadRes = loadSchema();
      if (!loadRes) return {Errors::unknownDatabase};
      string tableName = aSchema.getName();
      if (schemas.count(tableName)) return StatusResult{Errors::tableExists};
      // create the block and write the block
      StorageBlock theBlock{BlockType::entity_block};
      theBlock.header.id = aSchema.getBlockNum();
      ostringstream out;
      StatusResult encode = aSchema.encode(out);  // encode the schema information
      int pos = 0;    // block data position pointer
      strcpy(theBlock.data, out.str().c_str());
      pos += out.str().size();
      for (Attribute attr : aSchema.getAttributes()) {
        ostringstream out;
        StatusResult attrEncode = attr.encode(out);
        strcpy(theBlock.data + pos, out.str().c_str());
        pos += out.str().size();
      }   // complete block write
      StatusResult addSchemaResult = storage.addBlock(theBlock, tableName);

      schemas[tableName] = addSchemaResult.value;
      aSchema.setBlockNum(addSchemaResult.value);
      return addSchemaResult;
    }

    StatusResult Database::showTables(ostream &anOutput) {
      StatusResult loadRes = loadSchema();
      if (!loadRes) return {Errors::unknownDatabase};
      StatusResult res{Errors::noError};
      try {
        View* view = new ShowTableView(storage);
        view->show(anOutput);
        delete view;
      } catch(...) {
        res.code = Errors::unknownError;
      }
      return res;
    }

    StatusResult Database::dropTable(const string &aName) {
      StatusResult loadRes = loadSchema();
      if (!loadRes) return {Errors::unknownDatabase};
      if (schemas.count(aName)) {
        deleteRow(aName);
        uint32_t blockNum = schemas.at(aName);
        StorageBlock freeBlock{BlockType::free_block};
        storage.writeBlock(freeBlock, blockNum);
        schemas.erase(aName);
        return StatusResult{Errors::noError};
      } else return StatusResult{Errors::unknownTable};
    }

    Schema Database::getSchema(string& aName) {
      StatusResult queryRes = activeDB->getSchemaBlockNum(aName);
      if (!queryRes) return Schema("");
      int blockNumber = queryRes.value;
      StorageBlock curBlock;
      StatusResult readRes = activeDB->getStorage().readBlock(curBlock , blockNumber);
      if (!readRes) return Schema("");
      Schema curSchema = Schema("");
      StatusResult decodeResult = Schema::decode(curSchema, curBlock.data);
      return curSchema;
    }

    StatusResult Database::insert(Row& aRow, string aName) {
      StatusResult load = loadSchema();
      if (!load) return {Errors::unknownDatabase };
      if (schemas.count(aName)) {
        StatusResult freeBlockNumResult = storage.findFreeBlockNum();
        aRow.setBlockNum(freeBlockNumResult.value);
        Schema curSchema = getSchema(aName);
        // check if data(all the key values with attr) is valid
        if (!curSchema.validRow(aRow.getData())) return StatusResult{Errors::invalidAttribute};
        for (auto it = aRow.getData().begin(); it != aRow.getData().end(); ++it) {
          if (curSchema.getPrimaryKeyName() == it->first) {
            it->second.value = curSchema.getNextAutoIncrementValue();
          }
        }
        StorageBlock curBlock(BlockType::data_block);
        string tableName = aName + "@";
        strcpy(curBlock.data, tableName.c_str());
        StatusResult encodeRes = curSchema.encodeKeyValues(curBlock, aRow.getData());
        if (encodeRes) storage.addBlock(curBlock);
        return StatusResult{Errors::noError};
      } else {
        return StatusResult{Errors::unknownTable};
      }
    }

    StatusResult Database::deleteRow(string aName) {
      StatusResult load = loadSchema();
      if (schemas.count(aName)) {
        Schema curSchema = getSchema(aName);
        StorageBlock curBlock;
        for (uint32_t i = 0; i < storage.getTotalBlockCount(); ++i) {
          StatusResult readRes = storage.readBlock(curBlock, i);
          if (readRes) {
            if (curBlock.header.type == 'D' && string(curBlock.data).find("@") != -1 && string(curBlock.data).substr(0,string(curBlock.data).find("@")) == curSchema.getName()) {
              // current block is row block
              StorageBlock freeBlock{BlockType::free_block};
              storage.writeBlock(freeBlock, i);
            }
          }
        }
        return StatusResult{Errors::noError};
      } else {
        return StatusResult{Errors::unknownTable};
      }
    }
}
