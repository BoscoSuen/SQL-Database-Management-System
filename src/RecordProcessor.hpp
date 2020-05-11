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
#include "Database.hpp"
#include <unordered_set>

namespace ECE141 {

  class RecordProcessor : public CommandProcessor {
  public:
    RecordProcessor (CommandProcessor* aNext = nullptr);
    
    virtual ~RecordProcessor();
    
    virtual Statement* getStatement (Tokenizer& aTokenizer);
    virtual StatusResult interpret (const Statement &aStatement);
  
    StatusResult insert(Row& aRow, string aTableName);
    StatusResult update();
    StatusResult deleteRow(string aTableName);  // cannot use delete(), weird...
//  protected:
  };

  class RecordStatement : public Statement {
  public:
    RecordStatement(Keywords key , RecordProcessor* pointer):Statement(key),processor(pointer){}
    
  protected:
    RecordProcessor* processor;
    std::string tableName;
  };

  class InsertRecordStatement : public RecordStatement {
    public:
      InsertRecordStatement(Keywords key, RecordProcessor* pointer) : RecordStatement(key , pointer) {};
      StatusResult parse(Tokenizer& aTokenizer);
      StatusResult run(std::ostream& aStream) const;
      StatusResult parseIdentifier(Tokenizer& aTokenizer);
      StatusResult parseAttributeList(Tokenizer& aTokenizer);
      StatusResult parseValueList(Tokenizer& aTokenizer);
      StatusResult parseSingleValue(Tokenizer& aTokenizer);
      std::string getTableName();
      std::vector<std::string> getProperties();
      std::vector<Row*> getRows();
    protected:
      std::vector<std::string> properties;
      std::vector<Row*> rows;
  };

  class DeleteRecordStatement : public RecordStatement {
  public:
    DeleteRecordStatement(Keywords key, RecordProcessor* pointer);
    
    StatusResult parse(Tokenizer& aTokenizer);
    
    StatusResult run(std::ostream& aStream) const;
  };

}

#endif /* RecordProcessor_hpp */
