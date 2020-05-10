//
//  View.cpp
//  Assignment3
//
//  Created by Hongxiang Jiang on 2020/4/20.
//  Copyright © 2020 Hongxiang Jiang. All rights reserved.
//

#include "View.hpp"
#include <stdio.h>
#include <unordered_map>
#include <cstdio>

/*
namespace ECE141 {
  DescView::DescView(Storage& aStorage) : stream(nullptr), storage(aStorage) {}

  bool DescView::show(std::ostream &aStream) {
    static std::unordered_map<char, std::string> BlockTypeStrings{
            {'T', "Meta"},
            {'D', "Data"},
            {'E', "Entity"},
            {'F', "Free"},
            {'I', "Index"},
            {'V', "Unknow"},
    };
    bool status = true;
    try {
      StorageBlock curBlock;
      aStream << "Blk#  Type    Other" << std::endl;
      aStream << "----------------------------" << std::endl;
      for (uint32_t i = 0; i < storage.getTotalBlockCount(); ++i) {
        StatusResult readRes = storage.readBlock(curBlock, i);
        if (readRes) {
          if (curBlock.header.type == 'F') {
            continue;
          } else {
            aStream << i << " " << BlockTypeStrings[curBlock.header.type] << std::endl;
          }
        }
      }
    } catch(...) {
      status = false;
    }
    return status;
  }
}
*/