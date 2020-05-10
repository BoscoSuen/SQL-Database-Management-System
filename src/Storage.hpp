//
//  Storage.hpp
//  Assignment2
//
//  Created by rick gessner on 4/5/20.
//  Copyright © 2020 rick gessner. All rights reserved.
//

#ifndef Storage_hpp
#define Storage_hpp

#include <stdio.h>
#include <filesystem>

namespace ECE141 {

  // USE: Our main class for managing storage...
  class StorageInfo {
  public:
    
    static const char* getDefaultStoragePath();

  };

}

#endif /* Storage_hpp */
