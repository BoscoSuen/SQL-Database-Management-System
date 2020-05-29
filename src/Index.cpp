//
// Created by Suen on 5/15/20.
//

#include "Index.hpp"

namespace ECE141 {
  Index& Index::addKeyValue(const ValueType &aKey, uint32_t aValue) {
    index[aKey] = aValue;
    return *this;
  }

  Index& Index::removeKeyValue(const ValueType &aKey) {
    if (index.count(aKey)) index.erase(aKey);
    return *this;
  }

  bool Index::contains(const ValueType &aKey) {
    return index.count(aKey);
  }

  uint32_t Index::getValue(const ValueType &aKey) {
    return index.at(aKey);
  }

  Index::IndexType Index::getIndex() {
    return index;
  }
}
