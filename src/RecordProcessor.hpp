//
//  RecordProcessor.hpp
//  Assignment5
//
//  Created by Hongxiang Jiang on 2020/5/4.
//  Copyright © 2020 Hongxiang Jiang. All rights reserved.
//

#ifndef RecordProcessor_hpp
#define RecordProcessor_hpp

#include <stdio.h>
#include "CommandProcessor.hpp"
#include "Tokenizer.hpp"
#include "keywords.hpp"
#include "Row.hpp"
#include "Value.hpp"
#include <unordered_set>
#include "Database.hpp"
#include "Filters.hpp"
#include "Timer.hpp"
#include "IndexProcessor.hpp"

namespace ECE141 {
  class Filters;
  class RecordProcessor : public CommandProcessor {
  public:
    RecordProcessor (CommandProcessor* aNext = nullptr);
    
    virtual ~RecordProcessor();
    
    virtual Statement* getStatement (Tokenizer& aTokenizer);
    virtual StatusResult interpret (const Statement &aStatement);
  
    StatusResult insert(Row& aRow, string aTableName);
    StatusResult update(string& schemaName, KeyValues& aKeyValues, Filters& aFilters);
    StatusResult deleteRow(string aTableName);  // cannot use delete(), weird...
    StatusResult selectRow(string schemaName, Filters& filters, vector<string> properties, string orderBy, int limit);
//  protected:
  };

  class RecordStatement : public Statement {
  public:
    RecordStatement(Keywords key , RecordProcessor* pointer):Statement(key),processor(pointer){}
    std::string getTableName() {return tableName;}
    std::vector<std::string> getProperties() {return properties;}
  protected:
    RecordProcessor* processor;
    mutable std::string tableName;
    mutable std::vector<std::string> properties;
  };

  class InsertRecordStatement : public RecordStatement {
    public:
      InsertRecordStatement(Keywords key, RecordProcessor* pointer) : RecordStatement(key , pointer) {}
      StatusResult parse(Tokenizer& aTokenizer);
      StatusResult run(std::ostream& aStream) const;
      StatusResult parseIdentifier(Tokenizer& aTokenizer);
      StatusResult parseAttributeList(Tokenizer& aTokenizer);
      StatusResult parseValueList(Tokenizer& aTokenizer);
      StatusResult parseSingleValue(Tokenizer& aTokenizer);

      std::vector<Row*> getRows();
    protected:
//      std::vector<std::string> properties;
      std::vector<Row*> rows;
  };

  class DeleteRecordStatement : public RecordStatement {
  public:
    DeleteRecordStatement(Keywords key, RecordProcessor* pointer);
    
    StatusResult parse(Tokenizer& aTokenizer);
    
    StatusResult run(std::ostream& aStream) const;
  };


  class SelectRecordStatement : public RecordStatement {
  public:
    SelectRecordStatement(Keywords key, RecordProcessor* pointer) : RecordStatement(key , pointer) , displayAll(false) , limit(-1) , orderBy(""){}
    // free the heap object of expression
    ~SelectRecordStatement() {}
    StatusResult  parse(Tokenizer& aTokenizer);
    StatusResult  run(std::ostream& aStream) const;
    Expressions&  getExpressions() {return expressions;}
  protected:
    // select .....          true   (display all the property)
    //                       false  (display only listed in properties)
    mutable bool             displayAll;
    // properties vector still exist, just in father class
    //std::vector<std::string> properties;
    
    // from ...              name for schema, in father class
    // std::string tableName;
    
    // where .....
    mutable Expressions      expressions;
    
    // order by .....        ""     (no order)
    //                       "..."  (order by this property)
    mutable std::string      orderBy;
    
    // limit ...             -1     (no limit , display all)
    //                       x      (has limit, display the first x)
    mutable int              limit;
    
    StatusResult parseProperties(Tokenizer& aTokenizer);
    StatusResult parseExpressions(Tokenizer& aTokenizer);
    StatusResult parseLimit(Tokenizer& aTokenizer);
    StatusResult parseOrder(Tokenizer& aTokenizer);
  };


  class UpdateRecordStatement : public RecordStatement {
  public:
    UpdateRecordStatement(Keywords key , RecordProcessor* pointer) : RecordStatement(key , pointer) {}
    ~UpdateRecordStatement(){ /*delete update;*/}
    StatusResult parse(Tokenizer& aTokenizer);
    StatusResult run(std::ostream& aStream) const;
  protected:
    mutable Expression*       update;
    mutable Expressions       expressions;
    
    StatusResult parseExpressions(Tokenizer& aTokenizer);
    Expression*  parseExpression(Tokenizer& aTokenizer);
  };
}

#endif /* RecordProcessor_hpp */
