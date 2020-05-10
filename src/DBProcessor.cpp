//
//  CommandProcessor.cpp
//  ECEDatabase
//
//  Created by rick gessner on 3/30/18.
//  Copyright © 2018 rick gessner. All rights reserved.
//

#include <iostream>
#include "DBProcessor.hpp"
#include "Tokenizer.hpp"
#include <memory>

#include <fstream>

namespace ECE141 {

//1. Names can contan only alphanumeric characters and must begin with an alphabetic character.
//2. Database names cannot begin with an underscore
//3. Names can contain the following special characters: 0 through 9, #, @ and $ but must not begin with these characters.
char charList[] = {'0' , '1' , '2' , '3' , '4' , '5' , '6' , '7' , '8' , '9' , '#' , '@' , '$' , '_'};
bool charInList (char x) {
    for (char c : charList) {
        if (x == c) return true;
    }
    return false;
}
bool DBNameChecker(std::string DBName){
    // first character check
    char firstChar = DBName[0];
    if (charInList(firstChar)) return false;
    // rest of them check
    for (int i = 0 ; i < DBName.size() ; i++) {
        char cur = DBName[i];
        if (i > 0 && charInList(cur)) continue;
        if (cur >= 'a' && cur <= 'z') continue;
        if (cur >= 'A' && cur <= 'Z') continue;
        return false;
    }
    return true;
};
class DBStatement : public Statement {
public:
    DBStatement(Keywords key = Keywords::unknown_kw , std::string name = "") : Statement(key) , DBName(name) {}
protected:
    std::string DBName;
};

class CreateDBStatement : public DBStatement {
public:
    CreateDBStatement (Keywords key = Keywords::create_kw ) : DBStatement(key) {}
    CreateDBStatement (Keywords key , std::string name) : DBStatement(key , name) {}
    // input    :   std::ostream
    // output   :   StatusResult
    // LOG      :   PRINT + CHECK (& CREATE) FILE
    StatusResult run(std::ostream &aStream) const {
        if (!DBNameChecker(DBName)){
            return StatusResult(Errors::illegalIdentifier , 115);
        }
        std::string path(StorageInfo::getDefaultStoragePath());
        std::filesystem::path p(path + "/" + DBName + ".db");
        if (std::filesystem::exists(p)){
            return StatusResult(Errors::databaseExists , 330);
        } else {
            std::ofstream file(p);
//            file << "nothing";
            file.close();
            aStream << "created database " << DBName << " (ok)" << std::endl;
            return StatusResult();
        }
    }
};

class DropDBStatement : public DBStatement {
public:
    DropDBStatement (Keywords key = Keywords::create_kw ) : DBStatement(key) {}
    DropDBStatement (Keywords key , std::string name) : DBStatement(key , name) {}
    // input    :   std::ostream
    // output   :   StatusResult
    // LOG      :   PRINT + CHECK (& DROP) FILE
    StatusResult run(std::ostream &aStream) const {
        if (!DBNameChecker(DBName)){
            return StatusResult(Errors::illegalIdentifier , 115);
        }
        std::string path(StorageInfo::getDefaultStoragePath());
        std::filesystem::path p(path + "/" + DBName + ".db");
        if (std::filesystem::exists(p)){
            std::filesystem::remove(p);
            aStream << "dropped database " << DBName << " (ok)" << std::endl;
            return StatusResult();
        } else {
            return StatusResult(Errors::unknownDatabase , 320);
        }
    }
};

class UseDBStatement : public DBStatement {
public:
    UseDBStatement (Keywords key = Keywords::create_kw ) : DBStatement(key) {}
    UseDBStatement (Keywords key , std::string name) : DBStatement(key , name) {}
    // input    :   std::ostream
    // output   :   StatusResult
    // LOG      :   PRINT + CHECK (& USE) FILE
    StatusResult run(std::ostream &aStream) const {
        if (!DBNameChecker(DBName)){
            return StatusResult(Errors::illegalIdentifier , 115);
        }
        std::string path(StorageInfo::getDefaultStoragePath());
        std::filesystem::path p(path + "/" + DBName + ".db");
        if (std::filesystem::exists(p)){
            aStream << "using database " << DBName << std::endl;
            return StatusResult();
        } else {
            return StatusResult(Errors::unknownDatabase , 320);
        }
    }
};

class DescribeDBStatement : public DBStatement {
public:
    DescribeDBStatement (Keywords key = Keywords::create_kw ) : DBStatement(key) {}
    DescribeDBStatement (Keywords key , std::string name) : DBStatement(key , name) {}
    // input    :   std::ostream
    // output   :   StatusResult
    // LOG      :   PRINT + CHECK (& DESCRIBE) FILE
    StatusResult run(std::ostream &aStream) const {
        if (!DBNameChecker(DBName)){
            return StatusResult(Errors::illegalIdentifier , 115);
        }
        // right now, do nothing
        std::cout << std::endl;
        return StatusResult();
    }
};

class ShowDBStatement : public DBStatement {
public:
    ShowDBStatement (Keywords key = Keywords::show_kw ) : DBStatement(key) {}
    // input    :   std::ostream
    // output   :   StatusResult
    // LOG      :   PRINT + CHECK FILE
    StatusResult run(std::ostream &aStream) const {
        std::string path(StorageInfo::getDefaultStoragePath());
        std::filesystem::path p(path + "/");
        for (auto& file : std::filesystem::directory_iterator(p)) {
            if (file.path().extension() == ".db")
                std::cout << std::string(file.path().stem()) << std::endl;
        }
        return StatusResult();
    }
};

DBCmdProcessor::DBCmdProcessor(CommandProcessor *aNext) : CommandProcessor(aNext) {
}
DBCmdProcessor::~DBCmdProcessor (){};

//create database
//drop database
//use database
//describe database
//show databases
Statement*    DBCmdProcessor::getStatement(Tokenizer &aTokenizer){
    Statement* curStatement = nullptr;
    if (aTokenizer.more() && aTokenizer.tokenAt(0).type == TokenType::keyword){
        Token firstToken = aTokenizer.current();
        aTokenizer.next();
        if (aTokenizer.current().type == TokenType::keyword){
            Token secondToken = aTokenizer.current();
            aTokenizer.next();
            if (secondToken.keyword == Keywords::database_kw){
                if (!aTokenizer.more()) return curStatement;
                std::string rest = "";
                while (aTokenizer.more()){
                    rest = rest + aTokenizer.current().data;
                    aTokenizer.next();
                }
//                Token thirdToken = aTokenizer.current();
//                aTokenizer.next();
                if (firstToken.keyword == Keywords::create_kw) { curStatement = new CreateDBStatement(Keywords::create_kw , rest); }
                else if (firstToken.keyword == Keywords::drop_kw) { curStatement = new DropDBStatement(Keywords::drop_kw , rest); }
                else if (firstToken.keyword == Keywords::use_kw) { curStatement = new UseDBStatement(Keywords::use_kw , rest); }
                else if (firstToken.keyword == Keywords::describe_kw) { curStatement = new DescribeDBStatement(Keywords::describe_kw , rest); }
            } else if (aTokenizer.size() == 2 && secondToken.keyword == Keywords::databases_kw) {
                if (firstToken.keyword == Keywords::show_kw) { curStatement = new ShowDBStatement(); }
            }
        }
    }
    if (!curStatement) {
//        next = TableCmdProcessor();
        aTokenizer.restart();
    }
    return curStatement;
};

StatusResult  DBCmdProcessor::interpret(const Statement &aStatement){
    return aStatement.run(std::cout);
//    return StatusResult();
};

}
