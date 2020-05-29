//
//  IndexProcessor.cpp
//  Assignment8
//
//  Created by Hongxiang Jiang on 2020/5/28.
//  Copyright © 2020 Hongxiang Jiang. All rights reserved.
//

#include "IndexProcessor.hpp"

namespace ECE141 {
//class IndexProcessor : public CommandProcessor {
//public:
//  IndexProcessor(CommandProcessor* aNext = nullptr);
//
//  Statement* getStatement(Tokenizer& aTokenizer);
//  StatusResult interpret(const Statement& aStatement) ;
//
//  StatusResult showIndexes();
//};

  class ShowIndexStatement : public Statement {
  public:
    ShowIndexStatement (Keywords key) : Statement(key){}
    StatusResult parse(Tokenizer& aTokenizer) {return StatusResult();}
    StatusResult run(std::ostream &aStream) const {
      Database* activeDB = Database::getDBInstance();
      if (!activeDB) return StatusResult(Errors::noDatabaseSpecified , 0);
      return activeDB->showIndexes();
    }
  };

  IndexProcessor::IndexProcessor(CommandProcessor* aNext) : CommandProcessor(aNext){}
  
  Statement* IndexProcessor::getStatement(Tokenizer& aTokenizer) {
    std::vector<Keywords> keys = {Keywords::show_kw , Keywords::indexes_kw};
    if (!aTokenizer.keywordsMatch(keys)) {
      return nullptr;
    }
    return new ShowIndexStatement(Keywords::show_kw);
  }

  StatusResult IndexProcessor::interpret(const Statement& aStatement) {
    return aStatement.run(std::cout);
  }
}
