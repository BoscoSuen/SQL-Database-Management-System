//
//  DBProcessor.hpp
//  DataBase
//
//  Created by Hongxiang Jiang on 2020/4/10.
//  Copyright © 2020 Hongxiang Jiang. All rights reserved.
//

#ifndef DBProcessor_hpp
#define DBProcessor_hpp

#include <stdio.h>
#include "CommandProcessor.hpp"

#include "Storage.hpp"
#include <filesystem>

namespace ECE141 {

    class DBCmdProcessor : public CommandProcessor {
    public:

        DBCmdProcessor(CommandProcessor *aNext=nullptr);
        virtual ~DBCmdProcessor();

        virtual Statement*    getStatement(Tokenizer &aTokenizer);
        virtual StatusResult  interpret(const Statement &aStatement);

    };

}

#endif /* DBProcessor_hpp */
