//
//  CommandProcessor.cpp
//  ECEDatabase
//
//  Created by rick gessner on 3/30/18.
//  Copyright © 2018 rick gessner. All rights reserved.
//

#include <iostream>
#include "AppProcessor.hpp"
#include "Tokenizer.hpp"
#include <memory>

namespace ECE141 {
  using namespace std;

  class HelpStatement : public Statement {
  public:
      HelpStatement(Keywords aStatementType , Keywords key) : Statement(aStatementType) , key(key) {};
      unordered_map<Keywords, std::string> help_keywd_map = {
              {Keywords::help_kw, "\t -- help  - shows this list of commands\n"},
              {Keywords::version_kw, "\t -- version  -- shows the current version of this application\n"},
              {Keywords::quit_kw, "\t -- quit  -- terminates the execution of this DB application\n"},
              {Keywords::create_kw, "\t -- create database <name>  -- create a new database names as input <name>\n"},
              {Keywords::drop_kw,"\t -- drop database <name>   -- delete the database with specific name <name>\n"},
              {Keywords::use_kw, "\t -- use database <name>   -- switch to the database named as name <name>\n"},
              {Keywords::describe_kw, "\t -- describe database <name>  -- show the description of the database named as name <name>\n"},
              {Keywords::show_kw, "\t -- show databases  -- shows the list of databases available\n"}
      };
      StatusResult run(std::ostream &aStream) const {
          if (key == Keywords::all_kw) {
              aStream << "help -- the available list of commands shown below:" << std::endl;
              for (auto it = help_keywd_map.begin(); it != help_keywd_map.end(); ++it) {
                aStream << it->second;
              }
          } else {
              auto it = help_keywd_map.find(key);
              if (it == help_keywd_map.end()) aStream << "\t Unknown Keyword" << std::endl;
              else aStream << it->second;
          }
          return StatusResult();
      }

  protected:
      Keywords key;
  };



class VersionStatement : public Statement {
public:
    VersionStatement() :  Statement(Keywords::version_kw) {}
    StatusResult run(std::ostream &aStream) const {
        aStream << "version ECE141b-1" << std::endl;
        return StatusResult();
    }
};

class QuitStatement : public Statement {
public:
    QuitStatement() :  Statement(Keywords::quit_kw) {}
    StatusResult run(std::ostream &aStream) const {
      return StatusResult(Errors::userTerminated , 4998);
    }
};

  //.....................................

  AppCmdProcessor::AppCmdProcessor(CommandProcessor *aNext) : CommandProcessor(aNext) {
  }
  
  AppCmdProcessor::~AppCmdProcessor() {}
  
  // USE: -----------------------------------------------------
  StatusResult AppCmdProcessor::interpret(const Statement &aStatement) {
      //STUDENT: write code related to given statement
      return aStatement.run(std::cout);;
  }

    // input    :    tokenizer
    // output   :    invalid input -> null statement pointer
    //               valid   input -> real statement pointer
    // USE: factory to create statement based on given tokens...
  Statement* AppCmdProcessor::getStatement(Tokenizer &aTokenizer) {
    //STUDENT: Analyze tokens in tokenizer, see if they match one of the
    //         statements you are supposed to handle. If so, create a
    //         statement object of that type on heap and return it.
    
    //         If you recognize the tokens, consider using a factory
    //         to construct a custom statement object subclass.
    Statement* curStatement = nullptr;
    while (aTokenizer.more()) {
      Token curToken = aTokenizer.current();
      Keywords keyword = curToken.keyword;
      if (curToken.type == TokenType::keyword) {
        aTokenizer.next();
        if (keyword == Keywords::help_kw) curStatement = aTokenizer.more() ? new HelpStatement(Keywords::help_kw , aTokenizer.current().keyword) : new HelpStatement(Keywords::help_kw , Keywords::all_kw);
        else if (keyword == Keywords::version_kw) curStatement = new VersionStatement();
        else if (keyword == Keywords::quit_kw) curStatement = new QuitStatement();
      }
      aTokenizer.next();
    }
    if (!curStatement) {
        aTokenizer.restart();
        next = new DBCmdProcessor();
    }
    return curStatement;
  }
}
