//
//  Tokenizer.cpp
//  Database1
//
//  Created by rick gessner on 3/19/19.
//  Copyright © 2019 rick gessner. All rights reserved.
//

#include "Tokenizer.hpp"
#include "Helpers.hpp"
#include <exception>
#include <cstring>

namespace ECE141 {
  
  Tokenizer::Tokenizer(std::istream &anInput) : input(anInput), index(0)  {}
  
  Token& Tokenizer::tokenAt(size_t anOffset) {
    if(anOffset>=0 && anOffset<tokens.size()) {
      return tokens[anOffset];
    }
    throw std::out_of_range("invalid offset");
  }  
  
  bool Tokenizer::next(int anOffset) {
    if(index+anOffset<=size()) {
      index+=anOffset;
      return true;
    }
    return false;
  }
  
  Token& Tokenizer::current() {
    return tokens[index];
  }
  
  Token& Tokenizer::peek(int anOffset) {
    return tokenAt(index+anOffset);
  }

  bool Tokenizer::skipIf(Keywords aKeyword) {
    if (more() && (aKeyword==current().keyword)) {
      next(); //eat the target...
      return true;
    }
    return false;
  }
  
  // USE: skip a regular char, usually punctuation....
  bool Tokenizer::skipIf(char aChar) {
    if(more() && (aChar==current().data[0])) {
      next(); //eat the target...
      return true;
    }
    return false;
  }
  
  bool Tokenizer::skipIf(Operators anOperator) {
    Token &theToken=current();
    if(more() && (theToken.type==TokenType::operators) && (theToken.op==anOperator)) {
      next(); //eat the target...
      return true;
    }
    return false;
  }
    
  bool isWhitespace(char aChar) {
    static const char* theWS = " \t\r\n\b";
    return strchr(theWS,aChar);
  }
  
  bool isNumber(char aChar) {
    return isdigit(aChar) || '.'==aChar;
  }
  
  bool isAlphaNum(char aChar) {
    return isalnum(aChar) || '_'==aChar;
  }
  
  bool isQuote(char aChar) {
    return quote==aChar;
  }
  
  bool isOperator(char aChar) {
    return strchr("+-/*%=>!<.", aChar);
  }
  
  bool isSign(char aChar) {
    return strchr("+-", aChar);
  }
  
  bool isPunctuation(char aChar) {
    return strchr("()[]{}:;,", aChar);
  }
  
  std::string Tokenizer::readWhile(parseCallback aCallback) {
    std::string theResult;
    while (!input.eof() && (*aCallback)(input.peek())) {
      char theChar=input.get();
      theResult+=theChar;
    }
    return theResult;
  }
  
  std::string Tokenizer::readUntil(parseCallback aCallback, bool addTerminal) {
    std::string theResult;
    while (!input.eof() && !(*aCallback)(input.peek())) {
      theResult+=input.get();
    }
    if(addTerminal) {
      if(!input.eof()) {
        theResult+=input.get();
      }
    }
    return theResult;
  }
  
  std::string Tokenizer::readUntil(char aTerminalChar, bool addTerminal) {
    std::string theResult;
    while (!input.eof()) {
      char theChar=input.get();
      if(theChar!=aTerminalChar) {
        theResult+=theChar;
      }
      else break;
    }
    if(addTerminal) {
      if(!input.eof()) {
        theResult+=input.get();
      }
    }
    return theResult;
  }
  
  //run on input provided in constructor; produce vector of tokens...
  StatusResult Tokenizer::tokenize() {
    
    StatusResult theResult;
    char         theChar;
    
    while(!input.eof()) {
      theChar = input.peek();
      if(isPunctuation(theChar)) {
        Token theToken{TokenType::punctuation, Keywords::unknown_kw, Operators::unknown_op};
        theToken.data.push_back(input.get());
        tokens.push_back(theToken);
      }
      else if(isOperator(theChar)) {
        std::string temp;
        Token theToken{TokenType::operators};
        theToken.data.push_back(input.get());
        theToken.op=gOperators[temp];
        tokens.push_back(theToken);
      }
      else if(isNumber(theChar)) {
        Token theToken{TokenType::number, Keywords::unknown_kw, Operators::unknown_op};
        theToken.data=readWhile(isNumber);
        tokens.push_back(theToken);
      }
      else if(isQuote(theChar) || (apostrophe==theChar)) {
        input.get(); //skip first quote...
        Token theToken{TokenType::identifier, Keywords::unknown_kw, Operators::unknown_op};
        theToken.data=readUntil(theChar);
        tokens.push_back(theToken);
      }
      else if(isAlphaNum(theChar)) {
        std::string theString = readWhile(isAlphaNum);
        std::string temp(theString);
        std::transform(temp.begin(), temp.end(), temp.begin(), ::tolower);
        
        Token theToken{TokenType::keyword, Keywords::unknown_kw, Operators::unknown_op, temp};
        theToken.keyword = Helpers::getKeywordId(temp);
        if(Keywords::unknown_kw!=theToken.keyword) {
          tokens.push_back(theToken);
        }
        else {
          theToken.type=TokenType::identifier;
          theToken.data=theString;
          tokens.push_back(Token(theToken));
        }
      }
      readWhile(isWhitespace);
    }
    
    return theResult;
  }


  
  // USE: ----------------------------------------------
  
  void Tokenizer::dump() {
    for (auto &theToken : tokens) {
      std::cerr << "type ";
      switch(theToken.type) {
        case TokenType::punctuation:
          std::cerr << "punct " << theToken.data << "\n";
          break;
          
          //case TokenType::operators:
        case TokenType::operators:
          std::cerr << "operator " << theToken.data << "\n";
          break;
          
        case TokenType::number:
          std::cerr << "number " << theToken.data << "\n";
          break;
          
        case TokenType::string:
          std::cerr << "string " << theToken.data << "\n";
          break;
          
        case TokenType::identifier:
          std::cerr << "identifier " << theToken.data << "\n";
          break;
          
        case TokenType::keyword:
          std::cerr << "keyword " << theToken.data << "\n";
          break;
          
        default:
          break;
      }
    }
  }
  
  
}
