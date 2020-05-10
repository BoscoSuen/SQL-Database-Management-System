//
//  Database.hpp
//  ECEDatabase
//
//  Created by rick gessner on 3/30/18.
//  Copyright © 2018 rick gessner. All rights reserved.
//

#ifndef Database_hpp
#define Database_hpp

#include <stdio.h>
#include <string>
#include <iostream>
#include "Storage.hpp"
#include "View.hpp"

using namespace std;

namespace ECE141 {
  class View;
  class Database  {
  public:
    
    Database(const std::string aPath, CreateNewStorage);
    Database(const std::string aPath, OpenExistingStorage);
    ~Database();
    
    Storage&          getStorage() {return storage;}
    std::string&      getName() {return name;}

//    StatusResult      descDB(ostream &output);
    
  protected:
    
    std::string     name;
    Storage         storage;
  };
  
}

#endif /* Database_hpp */
