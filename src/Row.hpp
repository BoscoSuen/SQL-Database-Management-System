//
//  Row.hpp
//  Assignment4
//
//  Created by rick gessner on 4/19/20.
//  Copyright © 2020 rick gessner. All rights reserved.
//

#ifndef Row_hpp
#define Row_hpp

#include <stdio.h>
#include <string>
#include <utility>
#include <variant>
#include <vector>
#include "Storage.hpp"


namespace ECE141 {

  class Row : public Storable {
  public:

    Row(int32_t blockNum = 0);
    Row(const Row &aRow);
    ~Row();
    Row& operator=(const Row &aRow);
    bool operator==(Row &aCopy) const;
    
    KeyValues copyData() const {return data;}
    KeyValues& getData() {return data;}

    void setBlockNum(uint32_t blockNum);

    uint32_t getBlockNum();
    
    void insert(KeyValue pair) {data.insert(pair);};
    
    virtual StatusResult  encode(std::ostringstream &aWriter) const;
    
    StatusResult decode(Row& row , std::string str);
      //STUDENT: What other methods do you require?
    Value getData(string str);
    vector<string> split(string str,string pattern);
        
  protected:
    KeyValues data;  //you're free to change this if you like...
    int32_t blockNum;
  };

}

#endif /* Row_hpp */
