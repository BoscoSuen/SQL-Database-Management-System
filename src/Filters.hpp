//
//  Filters.hpp
//  RGAssignment6
//
//  Created by rick gessner on 5/4/20.
//  Copyright © 2020 rick gessner. All rights reserved.
//

#ifndef Filters_h
#define Filters_h

#include <stdio.h>
#include <vector>
#include <string>
#include "Errors.hpp"
#include "Value.hpp"
#include "Tokenizer.hpp"

namespace ECE141 {
  
  class Row;
  class Schema;
  
  struct Operand {
    Operand() {}
    Operand(std::string &aName, TokenType aType, ValueType &aValue, std::string &aSchemaName, uint32_t anId=0)
      : attrName(aName), type(aType), entityId(anId), schemaName(aSchemaName), value(aValue) {}
    
    TokenType   type; //so we know if it's a field, a const (number, string)...
    std::string attrName; //for named attr. in schema
    ValueType   value;
    std::string schemaName; //table's name
    
    uint32_t    entityId;
  };
  
  //---------------------------------------------------

  struct Expression {
    Operand     lhs;
    Operand     rhs;
    Operators   op;
    
    Expression(Operand &aLHSOperand, Operators anOp, Operand &aRHSOperand)
      : lhs(aLHSOperand), op(anOp), rhs(aRHSOperand) {}
    
    bool operator()(KeyValues &aList);
  };
  
  //---------------------------------------------------

  using Expressions = std::vector<Expression*>;

  //---------------------------------------------------

  class Filters {
  public:
    
    Filters(){}
    Filters(Expressions ex):expressions(ex){}
    Filters(const Filters &aFilters){expressions = aFilters.getExpressions();}
    ~Filters(){for (auto ex : expressions) delete ex;}
    
    size_t        getCount() const {return expressions.size();}
    bool          matches(KeyValues &aList) const;
    bool          matchIndex(ValueType index , std::string indexName) const;
    Filters&      add(Expression *anExpression);
    Expressions   getExpressions() const {return expressions;}
    
    
    friend class Tokenizer;
    
  protected:
    Expressions  expressions;
  };
   
}

#endif /* Filters_h */
