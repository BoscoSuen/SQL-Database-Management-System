//
//  RecordProcessor.cpp
//  Assignment5
//
//  Created by Hongxiang Jiang on 2020/5/4.
//  Copyright © 2020 Hongxiang Jiang. All rights reserved.
//

#include "RecordProcessor.hpp"

namespace ECE141 {
  // -----------------> insert validator <-----------------------
  class InsertValidator {
  public:
    InsertValidator(InsertValidator* aNext, InsertRecordStatement& aStatement) : next(aNext) , statement(aStatement){}
    
    virtual StatusResult validate() {
      StatusResult validRes = processValidate();
      if (!validRes) return validRes;
      
      if (next) return next->validate();
      else return StatusResult();
    };
    
    virtual StatusResult processValidate() = 0;
    
    virtual ~InsertValidator(){};
  protected:
    InsertValidator* next;
    InsertRecordStatement& statement;
  };

  class InsertDataValidator : public InsertValidator {
  public:
    InsertDataValidator(InsertValidator* aNext , InsertRecordStatement& aStatement) : InsertValidator(aNext, aStatement) , schema(Schema("")){
      Database* activeDB = Database::getDBInstance();
      StatusResult queryNumberRes = activeDB -> getSchemaBlockNum(statement.getTableName());
      StorageBlock block;
      activeDB -> getStorage().readBlock(block, queryNumberRes.value);
      Schema::decode(schema, block.data);
    }
    
    // 1. check if the input value have unmatch data type ---> keyValueMismatch
    // 2. check if the input property miss some not nullable attribute --> missingAttributes
    virtual StatusResult processValidate() override {
      AttributeList attList = schema.getAttributes();
      for (Attribute& att : attList){
        std::string attName = att.getName();
        StatusResult validRes = validSingleAttribute(att);
        if (!validRes) return validRes;
      }
      return StatusResult();
    }
    
    StatusResult validSingleAttribute(Attribute& att){
      for (Row* row : statement.getRows()) {
        KeyValues data = row->getData();
        auto iter = data.find(att.getName());
        // the attribute is not included inside the input command
        if (iter == data.end()) {
          StatusResult res = validNotFoundAttrbute(att , row);
          if (!res) return res;
        }
        // the attribute is included
        else {
          StatusResult res = validFoundAttribute(att , row);
          if (!res) return res;
        }
      }
      return StatusResult();
    }
    
    StatusResult validFoundAttribute(Attribute& att, Row* row) {
      if (att.getAutoIncreasing() == true) return StatusResult(syntaxError , 0);
      Value value;
      auto iter = row->getData().find(att.getName());
      std::string data = std::get<std::string>(iter->second.value);
      switch (att.getType()) {
        case DataType::int_type:
          try {
            iter->second = ValueType((uint32_t)(stoi(data)) , att.getType());
          } catch (...) {
            return StatusResult(Errors::invalidArguments , 0);
          }
          break;
        case DataType::float_type:
          try{
            iter->second = ValueType((stof(data)) , att.getType());
          } catch (...) {
            return StatusResult(Errors::invalidArguments , 0);
          }
        case DataType::datetime_type:
          // parse to see
          iter->second.type = att.getType();
          break;
        case DataType::varchar_type:
          if (data.size() > stoi(att.getVarCharOther())) return StatusResult(Errors::invalidArguments , 0);
          iter->second.type = att.getType();
          break;
        default:
          break;
      }
      return StatusResult();
    }
        
    StatusResult validNotFoundAttrbute(Attribute& att , Row* row){
      // auto increasing attribute will be auto-generated, add it to row
      if (att.getAutoIncreasing() == true) {
        uint32_t next = schema.getNextAutoIncrementValue();
        row->insert(std::make_pair(att.getName(), ValueType(next , att.getType())));
      }
      // attribute with default value will be auto-generated, add it to row
      else if (att.getHasDefault() == true) {
        std::string defaultString = att.getDefaultValue();
        Value value;
        switch (att.getType()) {
          case DataType::int_type:
            value = (uint32_t)(stoi(defaultString));
            break;
          case DataType::float_type:
            value = stof(defaultString);
            break;
          case DataType::bool_type:
            if (defaultString.compare("false")) value = false;
            else value = true;
            break;
          default:
            value = defaultString;
            break;
        }
        row->insert(std::make_pair(att.getName(), ValueType(value , att.getType())));
      }
      // attribute can't be null will return error
      else if (att.getNullable() == false) return StatusResult(Errors::missingAttributes , 0);
      else row->insert(std::make_pair(att.getName(),ValueType("NULL" , att.getType())));
      return StatusResult();
    }
  protected:
    Schema schema;
  };

  class InsertAttributeValidator : public InsertValidator{
  public:
    InsertAttributeValidator(InsertValidator* aNext, InsertRecordStatement& aStatement) : InsertValidator(aNext ,  aStatement) {}
    
    // 1. check is the input property have duplicate name
    // 2. check if the input property have unkonw attribute
    virtual StatusResult processValidate() {
      // -------- > get schema for the table < --------
      Database* activeDB = Database::getDBInstance();
      if (activeDB == nullptr) return StatusResult(Errors::noDatabaseSpecified , 0);
      StatusResult queryNumberRes = activeDB -> getSchemaBlockNum(statement.getTableName());
      if (!queryNumberRes) return queryNumberRes;
      StorageBlock block;
      activeDB -> getStorage().readBlock(block, queryNumberRes.value);
      Schema schema("");
      Schema::decode(schema, block.data);
      std::unordered_set<std::string> set;
      for (std::string property : statement.getProperties()) {
        if (set.find(property) != set.end()) return StatusResult(Errors::duplicateAttributes , 0);
        set.insert(property);
        Attribute att = schema.getAttribute(property);
        if (att.getType() == DataType::no_type) {
          delete &att;
          return StatusResult(Errors::unknownAttribute , 0);
        }
      }
      next = new InsertDataValidator(nullptr , statement);
      return StatusResult();
    }
  };

  // ------------> record manipulation statement <------------
  // insert into `table name` (attributes name...) values (data...) , (data...)
  StatusResult InsertRecordStatement::parse(Tokenizer& aTokenizer) {
    if (aTokenizer.size() <= 3) return StatusResult(syntaxError,0);
    
    Token& current = aTokenizer.current();
    if (current.type != TokenType::keyword || current.keyword != Keywords::insert_kw)
      return StatusResult(syntaxError , 0);
    
    aTokenizer.next();
    current = aTokenizer.current();
    if (current.type != TokenType::keyword || current.keyword != Keywords::into_kw)
      return StatusResult(syntaxError , 0);
    
    StatusResult parseRes;
    aTokenizer.next();
    parseRes = parseIdentifier(aTokenizer);
    if (!parseRes) {
      aTokenizer.restart();
      return parseRes;
    }
    
    aTokenizer.next();
    parseRes = parseAttributeList(aTokenizer);
    if (!parseRes) {
      aTokenizer.restart();
      return parseRes;
    }
    
    aTokenizer.next();
    current = aTokenizer.current();
    if (current.keyword != Keywords::values_kw)
      return StatusResult(syntaxError,0);
    
    aTokenizer.next();
    parseRes = parseValueList(aTokenizer);
    if (!parseRes) {
      aTokenizer.restart();
      return parseRes;
    }
    aTokenizer.end();
    
    InsertValidator* validator = new InsertAttributeValidator(nullptr , *this);
    validator->validate();
    delete validator;
    
    return StatusResult();
  }
  
  StatusResult InsertRecordStatement::run(std::ostream& aStream) const {
    for (Row* pointer : rows){
      StatusResult res = processor->insert(*pointer, tableName);
      if (!res) return res;
    }
    return StatusResult();
  }
  
  
  StatusResult InsertRecordStatement::parseIdentifier(Tokenizer& aTokenizer) {
    Token& current = aTokenizer.current();
    if (current.type != TokenType::identifier) return StatusResult(syntaxError , 0);
    tableName = current.data;
    return StatusResult();
  }
  
  StatusResult InsertRecordStatement::parseAttributeList(Tokenizer& aTokenizer) {
    Token& current = aTokenizer.current();
    if (current.type != TokenType::punctuation || current.data.compare("(") != 0)
      return StatusResult(syntaxError , 0);
    
    aTokenizer.next();
    while (aTokenizer.more()) {
      current = aTokenizer.current();
      if (current.type != TokenType::identifier) return StatusResult(syntaxError , 0);
      properties.push_back(current.data);
      
      aTokenizer.next();
      current = aTokenizer.current();
      if (current.type == TokenType::punctuation) {
        if (current.data.compare(")") == 0)
          break;
        else if (current.data.compare(",") == 0){
          aTokenizer.next();
          continue;
        }
      }
      return StatusResult(syntaxError,0);
    }
    
    return StatusResult();
  }
  
  StatusResult InsertRecordStatement::parseValueList(Tokenizer& aTokenizer) {
    while (aTokenizer.more()) {
      StatusResult parseRes = parseSingleValue(aTokenizer);
      if (!parseRes) {aTokenizer.restart(); return parseRes;}
      aTokenizer.next();
      if (aTokenizer.more()) {
        Token& comma = aTokenizer.current();
        if (comma.type != TokenType::punctuation || comma.data.compare(",") != 0)
          return StatusResult(syntaxError , 0);
        aTokenizer.next();
      }
    }
    return StatusResult();
  }
  
  StatusResult InsertRecordStatement::parseSingleValue(Tokenizer& aTokenizer) {
    if (aTokenizer.current().type != TokenType::punctuation || aTokenizer.current().data.compare("(") != 0)
      return StatusResult(syntaxError , 0);
    
    aTokenizer.next();
    
    Row* row = new Row();
    int length = (int)(properties.size());
    int index = 0;
    while (index < length) {
      DataType type = DataType::no_type;
      ValueType value(aTokenizer.current().data,type);
      auto tmp = make_pair(properties[index], value);
      row->insert(tmp);
      index++;
      
      aTokenizer.next();
      Token& token = aTokenizer.current();
      if (token.type != TokenType::punctuation) return StatusResult(syntaxError , 0);
      if (token.data.compare(")") == 0) break;
      else if (token.data.compare(",") == 0) {
        aTokenizer.next() ;
        continue;
      }
      else return StatusResult(syntaxError , 0);
    }
    
    if (index != length) return StatusResult(syntaxError , 0);
    rows.push_back(row);
    return StatusResult();
  }
    
  std::string InsertRecordStatement::getTableName() {return tableName;};
  std::vector<std::string> InsertRecordStatement::getProperties() {return properties;}
  std::vector<Row*> InsertRecordStatement::getRows() {return rows;};

  DeleteRecordStatement::DeleteRecordStatement(Keywords key, RecordProcessor* pointer) : RecordStatement(key , pointer) {};
  
  // delete from `table name`
  StatusResult DeleteRecordStatement::parse(Tokenizer& aTokenizer) {
    if (aTokenizer.size() != 3) return StatusResult(syntaxError , 0);
    aTokenizer.next();
    if (aTokenizer.current().keyword != Keywords::from_kw)
      return StatusResult(syntaxError , 0);
    aTokenizer.next();
    if (aTokenizer.current().type != TokenType::identifier) return StatusResult(syntaxError, 0);
    tableName = aTokenizer.current().data;
    return StatusResult();
  }
  
  StatusResult DeleteRecordStatement::run(std::ostream& aStream) const {
//    return StatusResult();
    Database* activeDB = Database::getDBInstance();
    if (!activeDB) return StatusResult(Errors::noDatabaseSpecified , 0);
    StatusResult queryNumberRes = activeDB->getSchemaBlockNum(tableName);
    if (!queryNumberRes) return queryNumberRes;
    return processor->deleteRow(tableName);
  }


  // --------- > record processor functions < ------------
  RecordProcessor::RecordProcessor(CommandProcessor* aNext) : CommandProcessor(aNext){};

  RecordProcessor::~RecordProcessor(){};

  Statement* RecordProcessor::getStatement(Tokenizer& aTokenizer) {
    Statement* curStatement = nullptr;
    if (aTokenizer.size() >= 1 && aTokenizer.current().type == TokenType::keyword) {
      Token& first = aTokenizer.current();
      if (first.keyword == Keywords::insert_kw) {
        curStatement = new InsertRecordStatement(Keywords::insert_kw , this);
      } else if(first.keyword == Keywords::delete_kw) {
        curStatement = new DeleteRecordStatement(Keywords::delete_kw , this);
      }
      
      if (curStatement) {
        if (curStatement->parse(aTokenizer)) {
          aTokenizer.end();
          return curStatement;
        } else {
          delete curStatement;
          curStatement = nullptr;
        }
      }
    }
    if (!curStatement) {
//      next = new NextProcessor();
      aTokenizer.restart();
    }
    return curStatement;
  }


  StatusResult RecordProcessor::interpret(const Statement& aStatement) {
    return aStatement.run(std::cout);
  }

  StatusResult RecordProcessor::insert(Row& aRow, string aTableName) {
    Database* activeDB = Database::getDBInstance();
    if (activeDB) {
      return activeDB->insert(aRow, aTableName);
    } else return StatusResult{Errors::noDatabaseSpecified};
  }

  StatusResult RecordProcessor::deleteRow(string aTableName) {
    Database* activeDB = Database::getDBInstance();
    if (activeDB) {
      return activeDB->deleteRow(aTableName);
    } else return StatusResult{Errors::noDatabaseSpecified};
  }
}
