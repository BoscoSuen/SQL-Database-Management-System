//
//  SQLProcessor.hpp
//  Assignment4
//
//  Created by rick gessner on 4/18/20.
//  Copyright © 2020 rick gessner. All rights reserved.
//

#ifndef SQLProcessor_hpp
#define SQLProcessor_hpp

#include <stdio.h>
#include "CommandProcessor.hpp"
#include "Tokenizer.hpp"
#include "Schema.hpp"
#include "Database.hpp"
#include "Attribute.hpp"

class Statement;
class Database; //define this later...


namespace ECE141 {
  using AttributeList = std::vector<Attribute>;

  class SQLProcessor : public CommandProcessor {
  public:
    
    SQLProcessor(CommandProcessor *aNext=nullptr);
    virtual ~SQLProcessor();
    
    virtual Statement*    getStatement(Tokenizer &aTokenizer);
    virtual StatusResult  interpret(const Statement &aStatement);
    
    StatusResult createTable(Schema &aSchema);
    StatusResult dropTable(const std::string &aName);
    StatusResult describeTable(const std::string &aName) const;
    StatusResult showTables();
    
/*  do these in next assignment
    StatusResult insert();
    StatusResult update();
    StatusResult delete();
*/
    
//  protected:
    //do you need other data members?
  };

}
#endif /* SQLProcessor_hpp */
