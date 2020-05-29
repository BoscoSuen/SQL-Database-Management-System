//
//  SQLProcessor.cpp
//  Assignment4
//
//  Created by rick gessner on 4/18/20.
//  Copyright © 2020 rick gessner. All rights reserved.
//

#include "SQLProcessor.hpp"

namespace ECE141 {

    char charTableList[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '#', '@', '$', '_'};

    bool charInTableList(char x) {
      for (char c : charTableList) {
        if (x == c) return true;
      }
      return false;
    }

    bool TableNameChecker(std::string DBName) {
      // first character check
      char firstChar = DBName[0];
      if (charInTableList(firstChar)) return false;
      // rest of them check
      for (int i = 0; i < DBName.size(); i++) {
        char cur = DBName[i];
        if (i > 0 && charInTableList(cur)) continue;
        if (cur >= 'a' && cur <= 'z') continue;
        if (cur >= 'A' && cur <= 'Z') continue;
        return false;
      }
      return true;
    }

    class TableStatement : public Statement {
    public:
        TableStatement(Keywords key) : Statement(key) {}
        TableStatement(Keywords key , SQLProcessor* pointer) : Statement(key) , processor(pointer) {}
    protected:
        std::string tableName;
        SQLProcessor* processor;
    };
// CREATE TABLE table1 (
// id INT PRIMARY KEY NOT NULL,
// name FLOAT DEFAULT 1.9
// )
    class CreateTableStatement : public TableStatement {
    public:
        CreateTableStatement(Keywords key , SQLProcessor* pointer) : TableStatement(key , pointer) , schema(Schema("")) {}
        // parse the information to schema member
        StatusResult parse(Tokenizer& aTokenizer) {
          // trying to parse the first three tokens
          StatusResult res = parseBegin(aTokenizer);
          if (!res) {
            aTokenizer.restart();
            return res;
          }
          // create a schema with name
          std::string tmpName = aTokenizer.current().data;
          schema = Schema(tmpName);
          // parse the attributes
          res = parseAttributes(aTokenizer);
          if (!res) {
            aTokenizer.restart();
            return res;
          }
          return StatusResult();
        }

        StatusResult parseAttributes(Tokenizer& aTokenizer) {
          // (
          aTokenizer.next();
          if (aTokenizer.current().type != TokenType::punctuation || aTokenizer.current().data.compare("(") != 0)
            return StatusResult(Errors::syntaxError , 0);
          // .... , .... ,
          aTokenizer.next();
          while (aTokenizer.more()) {
            StatusResult res = parseSingleAttribute(aTokenizer);
            if(!res){
              aTokenizer.restart();
              return res;
            }
            aTokenizer.next();
          }

          return StatusResult();
        }

        StatusResult parseSingleAttribute(Tokenizer& aTokenizer) {
          if (!aTokenizer.more()) return StatusResult(Errors::syntaxError , 0);
          Token& nameToken = aTokenizer.current();
          if (nameToken.type != TokenType::identifier) return StatusResult(Errors::syntaxError , 0);
          std::string name = nameToken.data;

          aTokenizer.next();
          if (!aTokenizer.more()) return StatusResult(Errors::syntaxError , 0);
          Token& typeToken = aTokenizer.current();
          if (typeToken.type != TokenType::keyword) return StatusResult(Errors::syntaxError , 0);
          DataType type = parseTypeToken(typeToken);
          if (type == DataType::no_type) return StatusResult(Errors::unknownType , 0);

          Attribute att = Attribute(name , type);

          aTokenizer.next();
          while (aTokenizer.more()) {
            Token& cur = aTokenizer.current();
            if (cur.type == TokenType::punctuation && (cur.data.compare(")") == 0 || cur.data.compare(",") == 0) )
              break;
            StatusResult res = parseOtherToken(aTokenizer , att);
            if (!res) {
              aTokenizer.restart();
              return res;
            } else {
              aTokenizer.next();
            }
          }
          schema.addAttribute(att);
          return StatusResult();
        }

        DataType parseTypeToken (Token& token) {
          static unordered_map<Keywords, DataType> map = {
                  {Keywords::integer_kw , DataType::int_type} ,
                  {Keywords::float_kw   , DataType::float_type} ,
                  {Keywords::boolean_kw , DataType::bool_type},
                  {Keywords::varchar_kw , DataType::varchar_type},
                  {Keywords::datetime_kw, DataType::datetime_type},
                  {Keywords::time_stamp_kw, DataType::datetime_type}
          };
          Keywords keyword = token.keyword;
          auto ret = map.find(keyword);
          if (ret != map.end())
            return ret->second;
          else
            return DataType::no_type;
        }

        StatusResult parseBegin(Tokenizer& aTokenizer) {
          if (aTokenizer.size() < 5)
            return StatusResult(Errors::syntaxError,0);
          if (aTokenizer.current().keyword != Keywords::create_kw)
            return StatusResult(Errors::syntaxError,0);
          aTokenizer.next();
          if (aTokenizer.current().type != TokenType::keyword || aTokenizer.current().keyword != Keywords::table_kw)
            return StatusResult(Errors::syntaxError , 0);
          aTokenizer.next();
          if (aTokenizer.current().type != TokenType::identifier)
            return StatusResult(Errors::syntaxError , 0);
          if (!TableNameChecker(aTokenizer.current().data))
            return StatusResult(Errors::illegalIdentifier , 0);
          return StatusResult();
        }

        StatusResult parseOtherToken(Tokenizer& aTokenizer , Attribute& att) {
          Token& cur = aTokenizer.current();
          if (cur.type == TokenType::punctuation && cur.data.compare("(") == 0 && att.getType() == DataType::varchar_type) return parseVarCharOther(aTokenizer,att);
          if (cur.type != TokenType::keyword)
            return StatusResult(Errors::syntaxError , 0);
          // NOT NULL
          if (cur.keyword == Keywords::not_kw)
            return parseNotNull(aTokenizer , att);
            // PRIMARY
          else if (cur.keyword == Keywords::primary_kw)
            return parsePrimary(aTokenizer, att);
            // AUTO INCREASE
          else if (cur.keyword == Keywords::auto_increment_kw)
            return parseAutoIncrease(aTokenizer, att);
            // DEFAULT $defaultValue$
          else if (cur.keyword == Keywords::default_kw)
            return parseDefaultValue(aTokenizer, att);

          // Not valid
          return StatusResult(Errors::syntaxError , 0);
        }

        StatusResult parseVarCharOther(Tokenizer& aTokenizer , Attribute& att) {
          aTokenizer.next();
          if (!aTokenizer.more()) return StatusResult(Errors::syntaxError , 0);
          att.setVarCharOther(aTokenizer.current().data);
          aTokenizer.next();
          if (!aTokenizer.more() || aTokenizer.current().data.compare(")") != 0) return StatusResult(Errors::syntaxError , 0);
          return StatusResult();
        }
      
        StatusResult parseNotNull(Tokenizer& aTokenizer  , Attribute& att) {
          aTokenizer.next();
          if (!aTokenizer.more()) return StatusResult(Errors::syntaxError , 0);
          Token& cur = aTokenizer.current();
          if (cur.type == TokenType::keyword && cur.keyword == Keywords::null_kw)
            att.setNullable(false);
          else
            return StatusResult(Errors::syntaxError , 0);
          return StatusResult();
        }

        StatusResult parsePrimary(Tokenizer &aTokenizer , Attribute& att) {
          aTokenizer.next();
          if (!aTokenizer.more()) return StatusResult(Errors::syntaxError , 0);
          Token& cur = aTokenizer.current();
          if (cur.type == TokenType::keyword && cur.keyword == Keywords::key_kw){
            att.setPrimary(true);
            att.setNullable(false);
            schema.setPrimaryKeyName(att.getName());
          }
          else
            return StatusResult(Errors::syntaxError , 0);
          return StatusResult();
        }

        StatusResult parseAutoIncrease(Tokenizer& aTokenizer , Attribute& att) {
          if (att.getType() != DataType::int_type) return StatusResult(syntaxError , 0);
          att.setAutoIncreasing(true);
          schema.setAutoIncreasing(true);
          return StatusResult();
        }

        StatusResult parseDefaultValue(Tokenizer& aTokenizer , Attribute& att) {
          aTokenizer.next();
          if (!aTokenizer.more()) return StatusResult(Errors::syntaxError , 0);
          Token& cur = aTokenizer.current();
          std::string defaultValue;

          if (att.getType() == DataType::float_type) {
            if (cur.type == TokenType::number){
              defaultValue = cur.data;
            } else return StatusResult(Errors::syntaxError , 0);
          } else if (att.getType() == DataType::int_type) {
            if (cur.type == TokenType::number) {
              defaultValue = cur.data;
            } else return StatusResult(Errors::syntaxError , 0);
          } else if (att.getType() == DataType::varchar_type) {
            defaultValue = cur.data;
          } else if (att.getType() == DataType::datetime_type) {
            defaultValue = cur.data;
          } else if (att.getType() == DataType::bool_type) {
            defaultValue = cur.data;
          } else {
            return StatusResult(Errors::syntaxError , 0);
          }
          att.setDefaultValue(defaultValue);
          att.setHasDefault(true);
          return StatusResult();

        }

        StatusResult run(std::ostream &aStream) const {
          return processor->createTable(schema);
        }

    protected:
        mutable Schema schema;
    };

    class DropTableStatement : public TableStatement {
    public:
        DropTableStatement(Keywords key , SQLProcessor* pointer) : TableStatement(key , pointer) {}

        StatusResult parse(Tokenizer& aTokenizer) {
          if (aTokenizer.size() != 3)
            return StatusResult(Errors::syntaxError , 0);
          if (aTokenizer.tokenAt(2).type != TokenType::identifier)
            return StatusResult(Errors::syntaxError , 0);
          tableName = aTokenizer.tokenAt(2).data;
          return StatusResult();
        }

        StatusResult run(std::ostream &aStream) const {
          return processor->dropTable(tableName);
        }
    };

    class DescribeTableStatement : public TableStatement {
    public:
        DescribeTableStatement(Keywords key , SQLProcessor* pointer) : TableStatement(key , pointer) {}

        StatusResult parse(Tokenizer& aTokenizer) {
          if (aTokenizer.size() != 2)
            return StatusResult(Errors::syntaxError , 0);
          if (aTokenizer.tokenAt(1).type != TokenType::identifier)
            return StatusResult(Errors::syntaxError , 0);
          tableName = aTokenizer.tokenAt(1).data;
          return StatusResult();
        }

        StatusResult run(std::ostream &aStream) const {
          return processor->describeTable(tableName);
        }
    };

    class ShowTablesStatement : public TableStatement {
    public:
        ShowTablesStatement(Keywords key , SQLProcessor* pointer) : TableStatement(key , pointer) {}

        StatusResult parse(Tokenizer& aTokenizer) {
          if (aTokenizer.size() != 2 || aTokenizer.tokenAt(1).keyword != Keywords::tables_kw)
            return StatusResult(Errors::syntaxError , 0);
          return StatusResult();
        }

        StatusResult run(std::ostream &aStream) const {
          return processor->showTables();
        }
    };

    SQLProcessor::SQLProcessor(CommandProcessor* aNext) : CommandProcessor(aNext) {}

    SQLProcessor::~SQLProcessor() {}

    Statement* SQLProcessor::getStatement(Tokenizer& aTokenizer) {
      Statement *curStatement = nullptr;
      if (aTokenizer.size() >= 1 && aTokenizer.tokenAt(0).type == TokenType::keyword) {
        Token& firstToken = aTokenizer.tokenAt(0);
        if (firstToken.keyword == Keywords::create_kw) {
          curStatement = new CreateTableStatement(Keywords::create_kw , this);
        } else if (firstToken.keyword == Keywords::drop_kw) {
          curStatement = new DropTableStatement(Keywords::drop_kw , this);
        } else if (firstToken.keyword == Keywords::describe_kw) {
          curStatement = new DescribeTableStatement(Keywords::describe_kw , this);
        } else if (firstToken.keyword == Keywords::show_kw) {
          curStatement = new ShowTablesStatement(Keywords::show_kw , this);
        }
        if (curStatement) {
          if (curStatement->parse(aTokenizer)) {
            aTokenizer.end();
            return curStatement;
          }else {
            delete curStatement;
            curStatement = nullptr;
          }
        }
      }
      if (!curStatement) {
        next = new RecordProcessor();
        aTokenizer.restart();
      }
      return curStatement;
    }

    StatusResult SQLProcessor::interpret(const Statement &aStatement) {
      return aStatement.run(std::cout);
    }

    // just for compile
    StatusResult SQLProcessor::createTable(Schema &aSchema){
      Database* activeDB = Database::getDBInstance();
      if (activeDB) {
        try {
          StatusResult createTableResult = activeDB->createTable(aSchema);
          return createTableResult;
//          ? createTableResult : StatusResult(Errors::invalidCommand)
        } catch(...) {
          return StatusResult(Errors::unknownIdentifier);
        }
      } else return StatusResult(Errors::noDatabaseSpecified);
    }


    StatusResult SQLProcessor::dropTable(const std::string &aName) {
      Database* activeDB = Database::getDBInstance();
      if (activeDB) {
        try {
          StatusResult dropTableRes = activeDB->dropTable(aName);
          return dropTableRes;
          // ? dropTableRes : StatusResult(Errors::invalidCommand);
        } catch(...) {
          return StatusResult(Errors::unknownIdentifier);
        }
      } else return StatusResult(Errors::noDatabaseSpecified);
    }

    StatusResult SQLProcessor::describeTable(const std::string &aName) const {
      Database* activeDB = Database::getDBInstance();
      if (!activeDB) return StatusResult(Errors::noDatabaseSpecified);
      StatusResult queryRes = activeDB->getSchemaBlockNum(aName);
      if (!queryRes) return queryRes;
      int blockNumber = queryRes.value;
      StorageBlock curBlock;
      StatusResult readRes = activeDB->getStorage().readBlock(curBlock , blockNumber);
      if (!readRes) return readRes;
      Schema res = Schema("");
      StatusResult decodeResult = Schema::decode(res, curBlock.data);
      if (!decodeResult) return decodeResult;
      DescTableView* view = new DescTableView(res);
      try {
        view->show(std::cout);
      } catch (...) {
        return StatusResult(Errors::unknownError , 0);
      }
      delete view;
      return StatusResult();
    }

    StatusResult SQLProcessor::showTables() {
      Database* activeDB = Database::getDBInstance();
      if (activeDB) {
        try {
          StatusResult showTablesRes = activeDB->showTables(cout);
          return showTablesRes ;
          // ? showTablesRes : StatusResult(Errors::invalidCommand);
        } catch(...) {
          return StatusResult(Errors::unknownIdentifier);
        }
      } else return StatusResult(Errors::noDatabaseSpecified);
    }
}
