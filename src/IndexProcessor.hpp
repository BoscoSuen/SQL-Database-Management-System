//
//  IndexProcessor.hpp
//  Assignment8
//
//  Created by Hongxiang Jiang on 2020/5/28.
//  Copyright © 2020 Hongxiang Jiang. All rights reserved.
//

#ifndef IndexProcessor_hpp
#define IndexProcessor_hpp

#include <stdio.h>
#include "CommandProcessor.hpp"
#include "Tokenizer.hpp"
#include "keywords.hpp"
#include "Database.hpp"

namespace  ECE141 {
  
  class IndexProcessor : public CommandProcessor {
  public:
    IndexProcessor(CommandProcessor* aNext = nullptr);
    
    Statement* getStatement(Tokenizer& aTokenizer);
    StatusResult interpret(const Statement& aStatement) ;
    
    StatusResult showIndexes();
  };

}

#endif /* IndexProcessor_hpp */
