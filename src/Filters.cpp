//
//  Filters.hpp
//  Assignement6
//
//  Created by rick gessner on 5/4/20.
//  Copyright © 2020 rick gessner. All rights reserved.
//


#include <stdio.h>
#include "Filters.hpp"
#include "Row.hpp"
#include "Schema.hpp"

namespace ECE141 {
  bool Expression::operator()(KeyValues& aList){
    auto iter = aList.find(lhs.attrName);
    if (iter == aList.end()) return false;
    ValueType leftValue = iter->second;
    
    // if the rhs is also an attribute, we shall add something
    ValueType rightValue = rhs.value;
    
    if (leftValue.type != rightValue.type) return false;
    try {
      switch (leftValue.type) {
        case DataType::int_type:
          if (op == Operators::equal_op && std::get<uint32_t>(leftValue.value) ==
          std::get<uint32_t>(rightValue.value))
            return true;
          if (op == Operators::gt_op && std::get<uint32_t>(leftValue.value) >
          std::get<uint32_t>(rightValue.value))
            return true;
          if (op == Operators::gte_op && std::get<uint32_t>(leftValue.value) >=
          std::get<uint32_t>(rightValue.value))
            return true;
          if (op == Operators::lt_op && std::get<uint32_t>(leftValue.value) <
          std::get<uint32_t>(rightValue.value))
            return true;
          if (op == Operators::lte_op && std::get<uint32_t>(leftValue.value) <=
          std::get<uint32_t>(rightValue.value))
            return true;
          break;
        case DataType::float_type:
          if (op == Operators::equal_op && std::get<float>(leftValue.value) ==
              std::get<float>(rightValue.value))
            return true;
          if (op == Operators::gte_op && std::get<float>(leftValue.value) >=
              std::get<float>(rightValue.value))
            return true;
          if (op == Operators::gt_op && std::get<float>(leftValue.value) >
              std::get<float>(rightValue.value))
            return true;
          if (op == Operators::lt_op && std::get<float>(leftValue.value) <
            std::get<float>(rightValue.value))
          return true;
          if (op == Operators::gte_op && std::get<float>(leftValue.value) <=
            std::get<float>(rightValue.value))
          return true;
        case DataType::bool_type:
          if (op == Operators::equal_op && std::get<bool>(leftValue.value) ==
              std::get<bool>(rightValue.value))
            return true;
        default:
          if (op == Operators::equal_op && std::get<string>(leftValue.value).compare(
              std::get<string>(rightValue.value)) == 0)
            return true;
          break;
      }
    } catch(...) {return false;}
    return false;
  }


  bool Filters::matches(KeyValues &aList) const{
    bool before = true;
    for (Expression* ex : expressions){
      if (ex->op == Operators::or_op){
        if (before == true) return true;
        else continue;
      }
      if (ex->op == Operators::and_op) {
        if (before == false) return false;
        else continue;
      }
      before = (*ex)(aList);
    }
    return before;
  }

  bool Filters::matchIndex(ValueType index , std::string indexName) const {
    if (expressions.size() >= 2 || expressions.size() == 0) return true;
    Expression* ex = expressions[0];
    bool res = true;
    if (ex->lhs.attrName.compare(indexName) == 0) {
      KeyValues tmp;
      tmp.insert(std::make_pair(indexName, index));
      res = (*ex)(tmp);
    }
    return res;
  }

  Filters& Filters::add(Expression *anExpression) {
    expressions.push_back(anExpression);
    return *this;
  }
}



