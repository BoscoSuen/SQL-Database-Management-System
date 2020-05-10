//
//  CommandProcessor.cpp
//  ECEDatabase
//
//  Created by rick gessner on 3/30/18.
//  Copyright © 2018 rick gessner. All rights reserved.
//

#include <iostream>
#include "DBProcessor.hpp"
#include "SQLProcessor.hpp"
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
          Token& token = aTokenizer.tokenAt(1);
          if (token.type != TokenType::keyword || token.keyword != Keywords::database_kw)
            return StatusResult(Errors::syntaxError, 140);
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

        StatusResult parse(Tokenizer &aTokenizer) {
          if (aTokenizer.size() != 2) return StatusResult(syntaxError , 0);
          Token& current = aTokenizer.tokenAt(1);
          if (current.type == TokenType::keyword && current.keyword == Keywords::databases_kw)
            return StatusResult();
          else
            return StatusResult(syntaxError , 0);
        }
      
        // input    :   std::ostream
        // output   :   StatusResult
        // LOG      :   PRINT + CHECK FILE
        StatusResult run(std::ostream &aStream) const {
          return processor->showDatabases();
        }
    };

    DBProcessor::DBProcessor(CommandProcessor *aNext) : CommandProcessor(aNext) {
    }

    DBProcessor::~DBProcessor() {
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
            curStatement = nullptr;
          }
        }
      }
      if (!curStatement) {
        next = new SQLProcessor();
        aTokenizer.restart();
      }
      return curStatement;
    }

    StatusResult DBProcessor::interpret(const Statement &aStatement) {
      return aStatement.run(std::cout);
      //    return StatusResult();
    }

    StatusResult DBProcessor::createDatabase(const std::string &aName) {
      StatusResult res = Database::createDB(aName);
      if (res)
        std::cout << "Database created" << std::endl;
      return res;
    }

    StatusResult DBProcessor::dropDatabase(const std::string &aName) {
      StatusResult res = Database::dropDB(aName);
      if (res)
        std::cout << "Database dropped" << std::endl;
      return res;
    }

    StatusResult DBProcessor::useDatabase(const std::string &aName) {
      return Database::useDB(aName);
    }

// Example ouput:
//  Blk#  Type    Other
//  ----------------------------
//  0     Meta
    StatusResult DBProcessor::describeDatabase (const std::string &aName){
      return Database::describeDB(aName);
    }

    StatusResult DBProcessor::showDatabases() const {
      return StatusResult{noError};
    }
}
