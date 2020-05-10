//
//  FolderReader.hpp
//  Database5
//
//  Created by rick gessner on 4/4/20.
//  Copyright © 2020 rick gessner. All rights reserved.
//

#ifndef FolderReader_h
#define FolderReader_h

#include <string>
#include <dirent.h>
#include <iostream>
#include <fstream>
#include <filesystem>

using namespace std;
namespace fs = std::filesystem;

namespace ECE141 {
  
  class FolderListener {
  public:
    virtual bool operator()(const std::string &aName)=0;
  };

  class TestListener : public FolderListener{
  public:
      TestListener() {}
      bool operator()(const std::string &aName) {
        std::cout << "A Path \"" << aName << "\" is called." << std::endl;
          return true;
      }
  };
  
  class FolderReader {
  public:

                  FolderReader(const char *aPath) : path(aPath) {}
    virtual       ~FolderReader() {}

    virtual bool  exists(const std::string &aPath) {
                    //STUDENT: add logic to see if FOLDER at given path exists.
                    return fs::exists(aPath);
                  }

    virtual void  each(FolderListener &aListener, const std::string &anExtension) const;

      
    std::string path;
  };
  
}

#endif /* FolderReader_h */
