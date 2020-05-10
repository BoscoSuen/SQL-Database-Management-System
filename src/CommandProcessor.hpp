//
//  CommandProcessor.hpp
//  Database
//
//  Created by rick gessner on 3/17/19.
//  Copyright © 2019 rick gessner. All rights reserved.
//

#ifndef CommandProcessor_hpp
#define CommandProcessor_hpp

#include <stdio.h>
#include <string>

#include "Statement.hpp"

namespace ECE141 {
  
  class Statement;
  class Tokenizer;
  class Database;
  
  //------ serves as base class for our 3 command-processors --------------

  class CommandProcessor { //processor interface
  public:
                          CommandProcessor(CommandProcessor *aNext=nullptr);
                          ~CommandProcessor();
    
    virtual StatusResult  processInput(Tokenizer &aTokenizer);
    virtual Database*     getActiveDatabase();

    virtual Statement*    getStatement(Tokenizer &aTokenizer)=0;
    virtual StatusResult  interpret(const Statement &aStatement)=0;

    
    CommandProcessor *next;
  };
   
}

#endif /* CommandProcessor_hpp */
