//
//  CommandProcessor.cpp
//  ECEDatabase
//
//  Created by rick gessner on 3/30/18.
//  Copyright © 2018 rick gessner. All rights reserved.
//

#include <iostream>
#include "DBProcessor.hpp"
#include "Tokenizer.hpp"
#include <memory>
#include <unordered_map>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include "View.hpp"

namespace ECE141 {
    char charList[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '#', '@', '$', '_'};

    bool charInList(char x) {
      for (char c : charList) {
        if (x == c) return true;
      }
      return false;
    }

    bool DBNameChecker(std::string DBName) {
      // first character check
      char firstChar = DBName[0];
      if (charInList(firstChar)) return false;
      // rest of them check
      for (int i = 0; i < DBName.size(); i++) {
        char cur = DBName[i];
        if (i > 0 && charInList(cur)) continue;
        if (cur >= 'a' && cur <= 'z') continue;
        if (cur >= 'A' && cur <= 'Z') continue;
        return false;
      }
      return true;
    };

    class DBStatement : public Statement {
    public:
        DBStatement(Keywords key) : Statement(key) {}

        DBStatement(Keywords key = Keywords::unknown_kw, DBProcessor *pointer = nullptr) : Statement(key),
                                                                                           processor(pointer) {}

        StatusResult parse(Tokenizer &aTokenizer) {
          return StatusResult();
        }

    protected:
        std::string DBName;
        DBProcessor *processor;
    };

    class CreateDBStatement : public DBStatement {
    public:
        CreateDBStatement(Keywords key = Keywords::create_kw, DBProcessor *pointer = nullptr) : DBStatement(key,
                                                                                                            pointer) {}

        // input    :   tokenizer
        // ouput    :   StatusResult
        // LOG      :   parse the name inside
        StatusResult parse(Tokenizer &aTokenizer) {
          if (aTokenizer.size() != 3) {
            return StatusResult(Errors::syntaxError, 140);
          }
          DBName = aTokenizer.tokenAt(2).data;
          if (!DBNameChecker(DBName)) {
            return StatusResult(Errors::illegalIdentifier, 115);
          }
          return StatusResult();
        }

        // input    :   std::ostream
        // output   :   StatusResult
        // LOG      :   Call processor to do the work
        StatusResult run(std::ostream &aStream) const {
          return processor->createDatabase(DBName);
        }
    };

    class DropDBStatement : public DBStatement {
    public:
        DropDBStatement(Keywords key = Keywords::drop_kw, DBProcessor *pointer = nullptr) : DBStatement(key, pointer) {}

        // input    :   tokenizer
        // ouput    :   StatusResult
        // LOG      :   parse the name inside
        StatusResult parse(Tokenizer &aTokenizer) {
          if (aTokenizer.size() != 3) {
            return StatusResult(Errors::syntaxError, 140);
          }
          DBName = aTokenizer.tokenAt(2).data;
          if (!DBNameChecker(DBName)) {
            return StatusResult(Errors::illegalIdentifier, 115);
          }
          return StatusResult();
        }

        // input    :   std::ostream
        // output   :   StatusResult
        // LOG      :   PRINT + CHECK (& DROP) FILE
        StatusResult run(std::ostream &aStream) const {
          return processor->dropDatabase(DBName);
        }
    };

    class UseDBStatement : public DBStatement {
    public:
        UseDBStatement(Keywords key = Keywords::use_kw, DBProcessor *pointer = nullptr) : DBStatement(key, pointer) {}

        // input    :   tokenizer
        // ouput    :   StatusResult
        // LOG      :   parse the name inside
        StatusResult parse(Tokenizer &aTokenizer) {
          if (aTokenizer.size() != 3) {
            return StatusResult(Errors::syntaxError, 140);
          }
          DBName = aTokenizer.tokenAt(2).data;
          if (!DBNameChecker(DBName)) {
            return StatusResult(Errors::illegalIdentifier, 115);
          }
          return StatusResult();
        }

        // input    :   std::ostream
        // output   :   StatusResult
        // LOG      :   PRINT + CHECK (& USE) FILE
        StatusResult run(std::ostream &aStream) const {
          return processor->useDatabase(DBName);
        }
    };

    class DescribeDBStatement : public DBStatement {
    public:
        DescribeDBStatement(Keywords key = Keywords::describe_kw, DBProcessor *pointer = nullptr) : DBStatement(key,
                                                                                                                pointer) {}

        StatusResult parse(Tokenizer &aTokenizer) {
          if (aTokenizer.size() != 3) {
            return StatusResult(Errors::syntaxError, 140);
          }
          DBName = aTokenizer.tokenAt(2).data;
          if (!DBNameChecker(DBName)) {
            return StatusResult(Errors::illegalIdentifier, 115);
          }
          return StatusResult();
        }

        // input    :   std::ostream
        // output   :   StatusResult
        // LOG      :   PRINT + CHECK (& DESCRIBE) FILE
        StatusResult run(std::ostream &aStream) const {
          return processor->describeDatabase(DBName);
        }
    };

    class ShowDBStatement : public DBStatement {
    public:
        ShowDBStatement(Keywords key = Keywords::show_kw, DBProcessor *pointer = nullptr) : DBStatement(key, pointer) {}

        // input    :   std::ostream
        // output   :   StatusResult
        // LOG      :   PRINT + CHECK FILE
        StatusResult run(std::ostream &aStream) const {
          return processor->showDatabases();
        }
    };

    DBProcessor::DBProcessor(CommandProcessor *aNext) : CommandProcessor(aNext) {
      activeDB = nullptr;
    }

    DBProcessor::~DBProcessor() {
      releaseDB();
    };

    //create database
    //drop database
    //use database
    //describe database
    //show databases
    Statement *DBProcessor::getStatement(Tokenizer &aTokenizer) {
      Statement *curStatement = nullptr;
      if (aTokenizer.more() && aTokenizer.tokenAt(0).type == TokenType::keyword) {
        Token &current = aTokenizer.current();
        if (current.keyword == Keywords::create_kw) {
          curStatement = new CreateDBStatement(Keywords::create_kw, this);
        } else if (current.keyword == Keywords::drop_kw) {
          curStatement = new DropDBStatement(Keywords::drop_kw, this);
        } else if (current.keyword == Keywords::use_kw) {
          curStatement = new UseDBStatement(Keywords::use_kw, this);
        } else if (current.keyword == Keywords::describe_kw) {
          curStatement = new DescribeDBStatement(Keywords::describe_kw, this);
        } else if (current.keyword == Keywords::show_kw) {
          curStatement = new ShowDBStatement();
        }
        if (curStatement) {
          if (curStatement->parse(aTokenizer)) {
            // up untill now, the statement is valid and good to go -> set the index
            aTokenizer.end();
            return curStatement;
          } else {
            delete curStatement;
          }
        }
      }
      if (!curStatement) {
        //        next = TableCmdProcessor();
        aTokenizer.restart();
      }
      return curStatement;
    }

    StatusResult DBProcessor::interpret(const Statement &aStatement) {
      return aStatement.run(std::cout);
      //    return StatusResult();
    }

    DBProcessor &DBProcessor::releaseDB() {
      if (activeDB != nullptr) {
        delete activeDB;
        activeDB = nullptr;
      }
      return *this;
    }

    StatusResult DBProcessor::createDatabase(const std::string &aName) {
      DBProcessor processor = releaseDB();  // only delete the current DB pointer, and can use the processor to load database
      activeDB = processor.loadDatabase(aName);
      if (activeDB) return StatusResult{Errors::databaseExists, 300};
      activeDB = new Database(aName, CreateNewStorage{});
      std::cout << "Database created" << std::endl;
      return StatusResult(Errors::noError, 5000);
    }

    Database *DBProcessor::loadDatabase(const std::string &aName) const {
      FolderReader *folderReader = new FolderReader(StorageInfo::getDefaultStoragePath());
      Database *target = nullptr;
      if (folderReader->exists(Storage::getDBPath(aName))) {
        target = new Database(aName, OpenExistingStorage{});
      }
      delete folderReader;
      return target;
    }

    StatusResult DBProcessor::dropDatabase(const std::string &aName) {
      FolderReader *folderReader = new FolderReader(StorageInfo::getDefaultStoragePath());
      if (!folderReader->exists(Storage::getDBPath(aName))) {
        return StatusResult(Errors::unknownDatabase, 0);
      }
      ostringstream out;
      out << StorageInfo::getDefaultStoragePath() << "/" << aName << ".db";
      filesystem::remove(out.str());
      if (activeDB && activeDB->getName().compare(aName))
        releaseDB();
      std::cout << "Database dropped" << std::endl;
      return StatusResult{noError};
    }

    StatusResult DBProcessor::useDatabase(const std::string &aName) {
      return StatusResult{noError};
    }

// Example ouput:
//  Blk#  Type    Other
//  ----------------------------
//  0     Meta
//    StatusResult DBProcessor::describeDatabase (const std::string &aName){
//
//      DBProcessor processor = releaseDB();  // only delete the current DB pointer, and can use the processor to load database
//      activeDB = processor.loadDatabase(aName);
//      if (!activeDB) return StatusResult(Errors::unknownDatabase , 0);
//      StatusResult viewResult = activeDB->descDB(cout);
//      return viewResult;
//    }

    StatusResult DBProcessor::describeDatabase(const std::string &aName) {
      static std::unordered_map<char, std::string> BlockTypeStrings{
              {'T', "Meta"},
              {'D', "Data"},
              {'E', "Entity"},
              {'F', "Free"},
              {'I', "Index"},
              {'V', "Unknown"},
      };
      DBProcessor processor = releaseDB();  // only delete the current DB pointer, and can use the processor to load database
      activeDB = processor.loadDatabase(aName);
      if (!activeDB) return StatusResult(Errors::unknownDatabase, 0);
      Storage &storage = activeDB->getStorage();
      StorageBlock curBlock;
      std::cout << "Blk#  Type    Other" << std::endl;
      std::cout << "----------------------------" << std::endl;
      for (uint32_t i = 0; i < storage.getTotalBlockCount(); ++i) {
        StatusResult readRes = storage.readBlock(curBlock, i);
        if (readRes) {
          if (curBlock.header.type == 'F') {
            continue;
          } else {
            std::cout << i << "     " << BlockTypeStrings[curBlock.header.type] << " " << std::endl;
          }
        }
      }
      return StatusResult{noError};
    }

    StatusResult DBProcessor::showDatabases() const {
      return StatusResult{noError};
    }
}
