//
//  main.cpp
//  Database2
//
//  Created by rick gessner on 3/17/19.
//  Copyright © 2019 rick gessner. All rights reserved.
//

#include <iostream>
#include <sstream>
#include <fstream>
#include <filesystem>

#include "AppProcessor.hpp"
#include "Tokenizer.hpp"
#include "Errors.hpp"
#include "Storage.hpp"
#include "FolderReader.hpp"


// USE: ---------------------------------------------

static std::map<int, std::string> theErrorMessages = {
  {ECE141::illegalIdentifier, "Illegal identifier"},
  {ECE141::unknownIdentifier, "Unknown identifier"},
  {ECE141::databaseExists, "Database exists"},
  {ECE141::tableExists, "Table Exists"},
  {ECE141::syntaxError, "Syntax Error"},
  {ECE141::unknownCommand, "Unknown command"},
  {ECE141::unknownDatabase,"Unknown database"},
  {ECE141::unknownTable,   "Unknown table"},
  {ECE141::unknownError,   "Unknown error"},
  {ECE141::noDatabaseSpecified, "No Database Specified"}
};

void showError(ECE141::StatusResult &aResult) {
  std::string theMessage="Unknown Error";
  if(theErrorMessages.count(aResult.code)) {
    theMessage=theErrorMessages[aResult.code];
  }
  std::cout << "Error (" << aResult.code << ") " << theMessage << "\n";
}
    // input    :   isStream & CommandProcessor (AppCmdProcessor)
    // output   :   Status Result
    //build a tokenizer, tokenize input, ask processors to handle...
ECE141::StatusResult handleInput(std::istream &aStream, ECE141::CommandProcessor &aProcessor) {
  ECE141::Tokenizer theTokenizer(aStream);
  
  //tokenize the input from aStream...
  ECE141::StatusResult theResult=theTokenizer.tokenize();
  while(theResult && theTokenizer.more()) {
    if(";"==theTokenizer.current().data) {
      theTokenizer.next();  //skip the ";"...
    }
    else theResult=aProcessor.processInput(theTokenizer);
  }
  return theResult;
}

//----------------------------------------------

int main(int argc, const char * argv[]) {
  //const char* path = ECE141::StorageInfo::getDefaultStoragePath(); 

  ECE141::AppCmdProcessor   theProcessor;  //add your db processor here too!
  ECE141::StatusResult      theResult{};
  ECE141::TestListener      aListener{};

//   Test FolderReader
//  std::string dir = std::filesystem::canonical(".");
//  std::cout << "Current Directory is: " << dir << std::endl;
//  ECE141::FolderReader reader(dir.c_str());
//  std::cout << "Test FolderReader" << std::endl;
//  std::cout << (reader.exists(dir.c_str()) ? "\t The current path exist OK" : "The currnt path does not exist") << std::endl;
//  std::string extension = ".hpp";
//  std::cout << "\t Test each: input extension is " << extension << std::endl;
//  reader.each(aListener, extension);

  if(argc>1) {
    std::ifstream theStream(argv[1]);
    return handleInput(theStream, theProcessor);
  }
  else {
    std::string theUserInput;
    bool running=true;
    do {
      std::cout << "\n> ";
      if(std::getline(std::cin, theUserInput)) {
        if(theUserInput.length()) {
          std::stringstream theStream(theUserInput);
          theResult=handleInput(theStream, theProcessor);
          if(!theResult) showError(theResult);
        }
        if(ECE141::userTerminated==theResult.code)
          running=false;
        // if return userTerminated status result -> QUIT!
      }
    }
    while (running);
  }
  

  return 0;
}

