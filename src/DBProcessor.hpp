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
#include "Database.hpp"
#include "FolderReader.hpp"
#include "Storage.hpp"
#include "View.hpp"
#include "SQLProcessor.hpp"
#include <filesystem>

namespace ECE141 {

    class DBProcessor : public CommandProcessor {
    public:

        DBProcessor(CommandProcessor *aNext=nullptr);
        virtual ~DBProcessor();

        virtual Statement*    getStatement(Tokenizer &aTokenizer);
        virtual StatusResult  interpret(const Statement &aStatement);

        StatusResult createDatabase(const std::string &aName);
        StatusResult dropDatabase (const std::string &aName);
        StatusResult useDatabase (const std::string &aName);
        StatusResult describeDatabase (const std::string &aName);
        StatusResult showDatabases () const;

//    protected:
//        Database *activeDB;
//        Database* loadDatabase(const std::string &aName) const;
//        DBProcessor& releaseDB();
    };

}

#endif /* DBProcessor_hpp */
