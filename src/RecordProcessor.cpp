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
  class Validator {
  public:
    Validator(Validator* aNext, Schema& aSchema) : next(aNext), schema(aSchema){}
    
    virtual StatusResult validate() {
      StatusResult validRes = processValidate();
      if (!validRes) return validRes;
      
      if (next) {
        StatusResult res = next->validate();
        delete next;
        return res;
      }
      else return StatusResult();
    }
    virtual StatusResult processValidate() = 0;
    virtual ~Validator(){}
  protected:
    Validator* next;
    RecordStatement* statement;
    Schema& schema;
  };

  // 1. check is the input property have duplicate name
  // 2. check if the input property have unkonw attribute
  class AttributeValidator : public Validator{
  public:
    AttributeValidator(Validator* aNext, Schema& aSchema , std::vector<std::string>& aProperties) : Validator(aNext , aSchema) , properties(aProperties) {}
    
    virtual StatusResult processValidate() {
      std::unordered_set<std::string> set;
      for (std::string property : properties) {
        if (set.find(property) != set.end()) return StatusResult(Errors::duplicateAttributes , 0);
        set.insert(property);
        Attribute att = schema.getAttribute(property);
        if (att.getType() == DataType::no_type) {
          return StatusResult(Errors::unknownAttribute , 0);
        }
      }
      return StatusResult();
    }
  protected:
    std::vector<std::string>& properties;
  };

  class InsertDataValidator : public Validator {
  public:
    InsertDataValidator(Validator* aNext , Schema& aSchema , InsertRecordStatement& aStatement) : Validator(aNext , aSchema) , insertStatement(aStatement){}
    
    // 1. check if the input value have unmatch data type ---> keyValueMismatch
    // 2. check if the input property miss some not nullable attribute --> missingAttributes
    virtual StatusResult processValidate() override {
      AttributeList attList = schema.getAttributes();
      for (Attribute& att : attList){
        std::string attName = att.getName();
        StatusResult validRes = validSingleAttribute(att);
        if (!validRes) return validRes;
      }
      // write back Schema Block
      return StatusResult();
    }
    
    StatusResult validSingleAttribute(Attribute& att){
      for (Row* row : insertStatement.getRows()) {
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
        case DataType::bool_type:
          if (data.compare("true") == 0 || data.compare("TRUE") == 0) iter->second = ValueType(true , DataType::bool_type);
          if (data.compare("false") == 0 || data.compare("FALSE") == 0) iter->second = ValueType(false , DataType::bool_type);
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
      else row->insert(std::make_pair(att.getName(),ValueType("" , att.getType())));
      return StatusResult();
    }
  protected:
      InsertRecordStatement& insertStatement;
  };

  class ExpressionDataValidator : public Validator {
  public:
    ExpressionDataValidator(Validator* aNext , Schema& aSchema , Expressions& ex) : Validator(aNext, aSchema) , expressions(ex){}
    
    virtual StatusResult processValidate() override {
      for (Expression* ex : expressions) {
        if (ex->op == Operators::and_op || ex->op == Operators::or_op)
          continue;
        // both hand of side can not be attribute
        if (ex->lhs.type != TokenType::identifier && ex->rhs.type != TokenType::identifier)
          return StatusResult(syntaxError , 0);
        // assume the lhs must be an attribute name
        DataType leftType = DataType::no_type;
        if (ex->lhs.type == TokenType::identifier){
          Attribute res = schema.getAttribute(ex->lhs.attrName);
          if (res.getType() == DataType::no_type){
            return StatusResult(Errors::unknownAttribute , 0);
          } else{
            leftType = res.getType();
            ex->lhs.schemaName = schema.getName();
          }
        }
        // rhs is also an attribute name
        if (ex->rhs.type == TokenType::identifier) {
          Attribute res = schema.getAttribute(ex->rhs.attrName);
          if (res.getType() != DataType::no_type) {
            ex->rhs.schemaName = schema.getName();
            if (res.getType() != leftType)
              return StatusResult(Errors::syntaxError , 0);
            else ex->rhs.schemaName = schema.getName();
          } else {
            if (leftType == DataType::bool_type) {
              ex->rhs.value.type = DataType::bool_type;
              if (ex->rhs.attrName.compare("TRUE") == 0) {
                ex->rhs.value.value = true;
              } else if (ex->rhs.attrName.compare("FALSE") == 0) {
                ex->rhs.value.value = false;
              } else return StatusResult(syntaxError , 0);
            } else {
              ex->rhs.value.value = ex->rhs.attrName;
              ex->rhs.value.type = DataType::varchar_type;
            }
            ex->rhs.attrName = "";
          }
        } else {
          ex->rhs.value.type = leftType;
          switch (leftType) {
            case DataType::int_type:
              if (ex->rhs.type == TokenType::number){
                try {
                  ex->rhs.value.value = (uint32_t)(stoi(ex->rhs.attrName));
                } catch(...) {
                  return StatusResult(syntaxError , 0);
                }
              } else return StatusResult(syntaxError , 0);
              break;
            case DataType::float_type:
              if (ex->rhs.type == TokenType::number) {
                ex->rhs.value.value = stof(ex->rhs.attrName);
              } else return StatusResult(syntaxError ,0);
            default:
              ex->rhs.value.value = ex->rhs.attrName;
              break;
          }
          ex->rhs.attrName = "";
        }
      }
      return StatusResult();
    }
    
  protected:
    Expressions& expressions;
  };

  // ------------> insert statement <------------
  // insert into `table name` (attributes name...) values (data...) , (data...)
  StatusResult InsertRecordStatement::parse(Tokenizer& aTokenizer) {
    if (aTokenizer.size() <= 3) return StatusResult(syntaxError,0);
    
    std::vector<Keywords> pattern = {
      Keywords::insert_kw ,
      Keywords::into_kw               };
    if (!aTokenizer.keywordsMatch(pattern)) {aTokenizer.restart(); return StatusResult(syntaxError,0);}
    
//    aTokenizer.next();
    StatusResult parseRes = parseIdentifier(aTokenizer);
    if (!parseRes) { aTokenizer.restart(); return parseRes; }
    
    aTokenizer.next();
    parseRes = parseAttributeList(aTokenizer);
    if (!parseRes) { aTokenizer.restart(); return parseRes; }
    
    aTokenizer.next();
    Token& current = aTokenizer.current();
    if (current.keyword != Keywords::values_kw)
      return StatusResult(syntaxError,0);
    
    aTokenizer.next();
    parseRes = parseValueList(aTokenizer);
    if (!parseRes) {
      aTokenizer.restart();
      return parseRes;
    }
    aTokenizer.end();
    
    
    Database* activeDB = Database::getDBInstance();
    if (activeDB == nullptr)
      return StatusResult(Errors::noDatabaseSpecified , 0);
    StatusResult queryNumberRes = activeDB -> getSchemaBlockNum(tableName);
    if (!queryNumberRes) return queryNumberRes;
    StorageBlock block;
    Schema schema("");
    activeDB -> getStorage().readBlock(block, queryNumberRes.value);
    Schema::decode(schema, block.data);
    
    Validator* validator = new AttributeValidator(
                new InsertDataValidator(nullptr , schema ,  *this) , schema , properties);
    StatusResult validRes = validator->validate();
    delete validator;
    
    // write it back
    std::string updateValue = std::to_string(schema.getPrevValue());
    int count = 0; int start = 0; int index = 0; int end = 0;
    while (block.data[index] != '\0') {
      if (block.data[index] == '|') count ++;
      if (count == 5) end = index;
      if (count == 4) start = index;
      index ++;
    }
    int delta = (int)(updateValue.length()) - (end - start) + 1;
    if (delta != 1){
      for (int i = index - 1; i >= end ; i-- )
        block.data[i + delta] = block.data[i];
    }
    strcpy(block.data + start, updateValue.c_str());
    block.data[end + delta - 1] = '|';
    
    activeDB->getStorage().writeBlock(block, queryNumberRes.value);
    
    if (!validRes) return validRes;
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
    
  std::vector<Row*> InsertRecordStatement::getRows() {return rows;}



  // -------------> delete statement <---------------
  DeleteRecordStatement::DeleteRecordStatement(Keywords key, RecordProcessor* pointer) : RecordStatement(key , pointer) {}
  
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

  // -------------> select statement <---------------
  StatusResult SelectRecordStatement::parse(Tokenizer& aTokenizer) {
    std::vector<Keywords> pattern = {Keywords::select_kw};
    if (!aTokenizer.keywordsMatch(pattern)) {aTokenizer.restart(); return StatusResult(syntaxError ,0);}
    
    // get properties
    StatusResult parseRes = parseProperties(aTokenizer);
    if (!parseRes) { aTokenizer.restart(); return parseRes; }
    
    // get table name
    if (!aTokenizer.more() || aTokenizer.current().type != TokenType::identifier) return StatusResult(syntaxError , 0);
    tableName = aTokenizer.current().data;
    // there is no string attached, return success
    aTokenizer.next();
    if (aTokenizer.more()) {
      // parse where to expression
      if (aTokenizer.more() && aTokenizer.current().keyword == Keywords::where_kw) {
        parseRes = parseExpressions(aTokenizer);
        if (!parseRes) { aTokenizer.restart(); return parseRes; }
      }
      
      // parse order by
      if (aTokenizer.more() && aTokenizer.current().keyword == Keywords::order_kw) {
        parseRes = parseOrder(aTokenizer);
        if (!parseRes) { aTokenizer.restart(); return parseRes; }
      }
      
      // parse limit
      if (aTokenizer.more() && aTokenizer.current().keyword == Keywords::limit_kw) {
        parseRes = parseLimit(aTokenizer);
        if (!parseRes) { aTokenizer.restart(); return parseRes; }
      }
    }
    return StatusResult();
  }
  
  StatusResult SelectRecordStatement::parseProperties(Tokenizer& aTokenizer) {
    if (aTokenizer.current().type == TokenType::operators) {
      if (aTokenizer.current().data.compare("*") == 0)
        displayAll = true;
      aTokenizer.next();
    } else if (aTokenizer.current().type == TokenType::identifier) {
      while (aTokenizer.more()) {
        Token& cur = aTokenizer.current();
        if (cur.type != TokenType::identifier) return StatusResult(syntaxError , 0);
        properties.push_back(cur.data);
        aTokenizer.next();
        if (aTokenizer.current().keyword == Keywords::from_kw) break;
        if (aTokenizer.current().type != TokenType::punctuation || aTokenizer.current().data.compare(",") != 0)
          return StatusResult(syntaxError , 0);
        aTokenizer.next();
      }
    } else return StatusResult(syntaxError , 0);
    if (!aTokenizer.more() || aTokenizer.current().keyword != Keywords::from_kw)
      return StatusResult(syntaxError , 0);
    aTokenizer.next();
    return StatusResult();
  }

  StatusResult SelectRecordStatement::parseExpressions(Tokenizer& aTokenizer) {
    aTokenizer.next();
    // ----  to be implemented --- mutilple expression
    while (aTokenizer.more() && aTokenizer.current().type != TokenType::keyword) {
      if (aTokenizer.remaining() < 3) return StatusResult(syntaxError , 0);
      // parse left hand side
      Operand lhs;
      lhs.type = aTokenizer.current().type;
      lhs.attrName = aTokenizer.current().data;
      aTokenizer.next();
      
      // parse operator
      if (aTokenizer.current().type != TokenType::operators) return StatusResult(syntaxError , 0);
      Operators op =aTokenizer.current().op;
      aTokenizer.next();
      
      // parse right hand side
      Operand rhs;
      rhs.type = aTokenizer.current().type;
      rhs.attrName = aTokenizer.current().data;
      aTokenizer.next();
      
      expressions.push_back(new Expression(lhs , op , rhs));
    }
    return StatusResult();
  }

  StatusResult SelectRecordStatement::parseOrder(Tokenizer& aTokenizer){
    std::vector<Keywords> pattern = {Keywords::order_kw , Keywords::by_kw};
    if (!aTokenizer.keywordsMatch(pattern)) return StatusResult(syntaxError , 0);
    if (!aTokenizer.more() || aTokenizer.current().type != TokenType::identifier) return StatusResult(syntaxError , 0);
    orderBy = aTokenizer.current().data;
    aTokenizer.next();
    return StatusResult();
  }

  StatusResult SelectRecordStatement::parseLimit(Tokenizer& aTokenizer){
    std::vector<Keywords> pattern = {Keywords::limit_kw};
    if (!aTokenizer.keywordsMatch(pattern)) return StatusResult(syntaxError , 0);
    if (!aTokenizer.more() || aTokenizer.current().type != TokenType::number) return StatusResult(syntaxError , 0);
    limit = stoi(aTokenizer.current().data);
    aTokenizer.next();
    return StatusResult();
  }

  StatusResult SelectRecordStatement::run(std::ostream& aStream) const {
    Database* activeDB = Database::getDBInstance();
    if (activeDB == nullptr)
      return StatusResult(Errors::noDatabaseSpecified , 0);
    StatusResult queryNumberRes = activeDB -> getSchemaBlockNum(tableName);
    if (!queryNumberRes) return queryNumberRes;
    StorageBlock block;
    activeDB -> getStorage().readBlock(block, queryNumberRes.value);
    Schema schema("");
    Schema::decode(schema, block.data);
    Validator* validator = new AttributeValidator(
                new ExpressionDataValidator(nullptr , schema , expressions) , schema , properties);
    StatusResult validRes = validator->validate();
    if (!validRes) return validRes;
    delete validator;
    
    Filters filters(expressions);
    // properties = member expressions
    // limit      = member limit
    // orderBy    = member orderBy
    // schemaName = member tableName
    return processor->selectRow(tableName , filters , properties , orderBy , limit);
//    return StatusResult();
  }

  // UPDATE users SET email='test@ucsd.edu' WHERE dept='CSE' AND name='anotherbar
  // -------------> select statement <---------------
  StatusResult UpdateRecordStatement::parse(Tokenizer& aTokenizer) {
    if (aTokenizer.size() < 6) return StatusResult(syntaxError , 0);
    
    if (aTokenizer.current().keyword != Keywords::update_kw){
      aTokenizer.restart(); return StatusResult(syntaxError ,0);
    }
    
    aTokenizer.next();
    if (aTokenizer.current().type != TokenType::identifier){
      aTokenizer.restart(); return StatusResult(syntaxError ,0);
    }
    tableName = aTokenizer.current().data;
    
    aTokenizer.next();
    if (aTokenizer.current().keyword != Keywords::set_kw){
      aTokenizer.restart(); return StatusResult(syntaxError ,0);
    }
    
    aTokenizer.next();
    if (aTokenizer.current().type != TokenType::identifier){
      aTokenizer.restart(); return StatusResult(syntaxError ,0);
    }
    properties.push_back(aTokenizer.current().data);
    
//    aTokenizer.next();
    update = parseExpression(aTokenizer);
    if (update == nullptr) {aTokenizer.restart(); return StatusResult(syntaxError , 0);}
    
    // no string attached
    if (aTokenizer.more()){
      if (aTokenizer.current().keyword != Keywords::where_kw)
        {aTokenizer.restart(); return StatusResult(syntaxError , 0);}
      
//      if (!aTokenizer.more())
//        {aTokenizer.restart(); return StatusResult(syntaxError , 0);}
      StatusResult parseRes = parseExpressions(aTokenizer);
      if (!parseRes) {aTokenizer.restart(); return StatusResult(syntaxError , 0);}
    }
    
    return StatusResult();
  }

  Expression* UpdateRecordStatement::parseExpression(Tokenizer& aTokenizer) {
    Operand lhs; Operand rhs;
    if (aTokenizer.current().keyword == Keywords::and_kw)
    {aTokenizer.next(); return new Expression(lhs , Operators::and_op , rhs);}
    if (aTokenizer.current().keyword == Keywords::or_kw)
    {aTokenizer.next(); return new Expression(lhs , Operators::or_op , rhs);}
    // parse left hand side
    lhs.type = aTokenizer.current().type;
    lhs.attrName = aTokenizer.current().data;
    aTokenizer.next();
    
    // parse operator
    if (aTokenizer.current().type != TokenType::operators) return nullptr;
    Operators op =aTokenizer.current().op;
    aTokenizer.next();
    
    // parse right hand side
    rhs.type = aTokenizer.current().type;
    rhs.attrName = aTokenizer.current().data;
    aTokenizer.next();
    
    return new Expression(lhs , op , rhs);
  }

  StatusResult UpdateRecordStatement::parseExpressions(Tokenizer& aTokenizer) {
    aTokenizer.next();
    // ----  to be implemented --- mutilple expression
    while (aTokenizer.more()) {
      Expression* ex = parseExpression(aTokenizer);
      if (ex == nullptr) return StatusResult(syntaxError , 0);
      expressions.push_back(ex);
    }
    return StatusResult();
  }

  StatusResult UpdateRecordStatement::run(std::ostream& aStream) const {
    Database* activeDB = Database::getDBInstance();
    if (activeDB == nullptr)
      return StatusResult(Errors::noDatabaseSpecified , 0);
    StatusResult queryNumberRes = activeDB -> getSchemaBlockNum(tableName);
    if (!queryNumberRes) return queryNumberRes;
    StorageBlock block;
    activeDB -> getStorage().readBlock(block, queryNumberRes.value);
    Schema schema("");
    Schema::decode(schema, block.data);
    // add upadate to expressions temperaryly
    expressions.push_back(update);
    Validator* validator = new AttributeValidator(
                new ExpressionDataValidator(nullptr , schema , expressions) , schema , properties);
    StatusResult validRes = validator->validate();
    expressions.pop_back();
    if (!validRes) return validRes;
    delete validator;
    
    Filters filters(expressions);
    
    KeyValues updateList = {std::make_pair(update->lhs.attrName , update->rhs.value)};
//    updateList.insert();
    
    return processor->update(tableName , updateList , filters);
//    return StatusResult();
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
      } else if (first.keyword == Keywords::select_kw) {
        curStatement = new SelectRecordStatement(Keywords::select_kw , this);
      } else if (first.keyword == Keywords::update_kw) {
        curStatement = new UpdateRecordStatement(Keywords::update_kw , this);
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
      next = new IndexProcessor();
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
      Timer timer;
      timer.start();
      StatusResult res = activeDB->deleteRow(aTableName);
      timer.stop();
//      clog << "delete time duration: " << timer.elapsed() << endl;
      cout << " (" << timer.elapsed() << " ms.)" << std::endl;
      return res;
    } else return StatusResult{Errors::noDatabaseSpecified};
  }

  StatusResult RecordProcessor::update(string& schemaName, KeyValues& aKeyValues, Filters& aFilters) {
    Database* activeDB = Database::getDBInstance();
    if (activeDB) {
      Timer timer;
      timer.start();
      Schema curSchema = activeDB->getSchema(schemaName);
      RowCollection rowCollection;
      vector<string> properties;
      // empty properties for update, and it will have no limit, limit = -1
      StatusResult selectResult = activeDB->selectRow(aFilters, curSchema, rowCollection, properties, -1);
      for (auto* row : rowCollection.getRows()) {
        if (!activeDB->updateRow(curSchema, aKeyValues, *row)) break;
      }
      timer.stop();
//      clog << "update time duration: " << timer.elapsed() << endl;
      cout << " (" << timer.elapsed() << " ms.)" << endl;
      return StatusResult{Errors::noError};
    } else return StatusResult{Errors::noDatabaseSpecified};
  }

  StatusResult RecordProcessor::selectRow(string schemaName, Filters& filters, vector<string> properties, string orderBy, int limit) {
    Timer timer;
    timer.start();
    Database* activeDB = Database::getDBInstance();
    Schema curSchema = activeDB->getSchema(schemaName);
    RowCollection rowCollection;
    StatusResult selectResult = activeDB->selectRow(filters, curSchema, rowCollection, properties, limit);
    if (!selectResult) return StatusResult{Errors::unknownDatabase};
    if (orderBy.size())  {
      rowCollection.order(curSchema, orderBy);
    }
    AttributeList attrs = curSchema.getAttributes();
    AttributeList displayAttr;
    if (properties.size() == 0)
      displayAttr = attrs;
    else {
      for (auto att : attrs) {
        if (find(properties.begin() , properties.end() , att.getName()) != properties.end()) {
          displayAttr.push_back(att);
        }
      }
    }
    timer.stop();
    
    
    if (View* view = new SelectTableView(displayAttr, rowCollection.getRows())) {
      view->show(cout);
      delete view;
    }
//    clog << "select time duration: " << timer.elapsed() << endl;
    cout << " (" << timer.elapsed() << " ms.)" << endl;
    return selectResult;
  }
}
