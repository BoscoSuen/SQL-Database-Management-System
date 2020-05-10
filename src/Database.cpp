//
//  Database.cpp
//  Database1
//
//  Created by rick gessner on 4/12/19.
//  Copyright © 2019 rick gessner. All rights reserved.
//

#include <sstream>
#include "Database.hpp"
#include "View.hpp"
#include "Storage.hpp"

//this class represents the database object.
//This class should do actual database related work,
//we called upon by your db processor or commands

namespace ECE141 {
  
  Database::Database(const std::string aName, CreateNewStorage)
    : name(aName), storage(aName, CreateNewStorage{}) {
  }
  
  Database::Database(const std::string aName, OpenExistingStorage)
    : name(aName), storage(aName, OpenExistingStorage{}) {
  }
  
  Database::~Database() {
  }

//  StatusResult Database::descDB(ostream &output) {
//    StatusResult res{Errors::noError};
//    try {
//      View* view = new DescView(storage);
//      view->show(output);
//      delete view;
//    } catch(...) {
//      res.code = Errors::unknownError;
//    }
//    return res;
//  }
}

