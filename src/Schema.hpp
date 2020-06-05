//
//  Schema.hpp
//  Assignment4
//
//  Created by rick gessner on 4/18/20.
//  Copyright © 2020 rick gessner. All rights reserved.
//

#ifndef Schema_hpp
#define Schema_hpp

#include <stdio.h>
#include <vector>
#include <iostream>
#include <functional>
#include "Attribute.hpp"
#include "Errors.hpp"
#include "Storage.hpp"
#include "Row.hpp"

namespace ECE141 {
  
  struct Block;
  struct Expression;
  class  Database;
  class  Tokenizer;
  class  Attribute;
  using StringList = std::vector<std::string>;
  using attribute_callback = std::function<bool(const Attribute &anAttribute)>;
  
  using AttributeList = std::vector<Attribute>;
  
  //STUDENT: If you're using the Storable interface, add that to Schema class?

  class Schema : public Storable {
  public:
                          Schema(const std::string aName);
                          Schema(const Schema &aCopy);
    
                          ~Schema();
        
    const std::string&    getName() const {return name;}
    const AttributeList&  getAttributes() const {return attributes;}
    uint32_t              getBlockNum() const {return blockNum;}
    void                  setBlockNum(uint32_t aNum) const {blockNum = aNum;}
    bool                  isChanged() {return changed;}
    
    Schema&               addAttribute(const Attribute &anAttribute);
    const Attribute&      getAttribute(const std::string &aName) const;
      
        //STUDENT: These methods will be implemented in the next assignment...
    
    //Value                 getDefaultValue(const Attribute &anAttribute) const;
    //StatusResult          validate(KeyValues &aList);
    
    std::string           getPrimaryKeyName() const;
    void                  setPrimaryKeyName(std::string name) {primaryKeyName = name;}
    DataType              getPrimaryKeyType() const;
    uint32_t              getNextAutoIncrementValue() {prevValue++; return prevValue - 1;}
    uint32_t              getPrevValue(){return prevValue;}
    void                  setAutoIncreasing(bool a) {AutoIncreasing = a; prevValue = 1;}

    void                  setChanged(bool a) {changed = a;}
    void                  setPrevValue(uint32_t number) {prevValue = number;}
    //STUDENT: Do you want to provide an each() method for observers?
    
    //friend class Database; //is this helpful?

    bool                  attrInSchema(string attrName);

    StatusResult    encode(std::ostringstream &aWriter) const;

    StatusResult    encodeKeyValues(StorageBlock& aBlock, KeyValues& data);
    
    static StatusResult decode(Schema& schema , std::string str);

    StatusResult    decodeWithProperties(StorageBlock& curBlock, vector<string>& properties, KeyValues& keyValues);


      bool                  validRow(KeyValues data);

  protected:
    
    AttributeList   attributes;
    std::string     name;
    mutable uint32_t        blockNum;  //storage location.
    bool            changed;
    
    std::string     primaryKeyName;
    bool            AutoIncreasing;
    uint32_t        prevValue;
  };
  
}
#endif /* Schema_hpp */
