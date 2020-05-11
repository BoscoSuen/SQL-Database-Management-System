//
//  Storage.cpp
//  Assignment2
//
//  Created by rick gessner on 4/5/20.
//  Copyright © 2020 rick gessner. All rights reserved.
//

#include "Storage.hpp"
#include <fstream>
#include <cstdlib>

namespace ECE141 {

    // USE: Our main class for managing storage...
    const char* StorageInfo::getDefaultStoragePath() {
      //STUDENT -- MAKE SURE TO SET AN ENVIRONMENT VAR for DB_PATH!
      //           This lets us change the storage location during autograder testing

      //WINDOWS USERS:  Use forward slash (/) not backslash (\) to separate paths.
      //                (Windows convert forward slashes for you)
      
//    const char* thePath = "/Users/suen/Desktop/ECE141B/ece141-sp20-assignment5-zhiqiang_sun_hongxiang_jiang/tempdb";
      const char* thePath = std::getenv("DB_PATH");
      return thePath;
//    return "/tmp";
    }

    //----------------------------------------------------------

    //path to the folder where you want to store your DB's...
    std::string getDatabasePath(const std::string &aDBName) {
      //build a full path (in default storage location) to a given db file..
      ostringstream out;
      out << StorageInfo::getDefaultStoragePath() << "/" << aDBName << ".db";
      return out.str();
    }

    // USE: ctor ---------------------------------------
    StatusResult Storage::addBlock(StorageBlock& aBlock) {
      StatusResult findFreeBlockRes = findFreeBlockNum();
      if (findFreeBlockRes) {
        // the find free block num should return an index in the StatusResult, in findFreeBlockRes.value
//      std::clog << " addBlock : find a free block and write the block there " << findFreeBlockRes.value << std::endl;
        return writeBlock(aBlock, findFreeBlockRes.value);
      }
      // there is no free blocks, need to append a new block
      stream.seekg(stream.tellg(),stream.beg);
      stream.seekp(0, stream.end);
      try {
//      std::clog << " addBlock : no free block, write it in the end" << std::endl;
        stream.write((char*)&aBlock, sizeof(StorageBlock));
      } catch(...) {
        return StatusResult(Errors::writeError);
      }
      return StatusResult();
    }

    StatusResult Storage::addBlock(StorageBlock &aBlock, string tableName) {
      StatusResult findFreeBlockRes = findFreeBlockNum();
      if (!findFreeBlockRes) return StatusResult{Errors::unknownDatabase};
      uint32_t blockNum = findFreeBlockRes.value;
      StatusResult writeBlockRes = writeBlock(aBlock, blockNum);
      writeBlockRes.value = blockNum;
      return writeBlockRes;
    }

    Storage::Storage(const std::string aName, CreateNewStorage) : block(BlockType::meta_block), name(aName) {
      //try to create a new db file in known storage location.
      //throw error if it fails...
      const char* thePath = getDatabasePath(name).c_str();
      try {
        stream.open(thePath, fstream::out);  // init and create file
        stream.close();
        stream.open(thePath, fstream::in | fstream::out | fstream::binary);
        if (isReady()) {
//        std::clog << " Storage Constructor : file is open and good to add block" << std::endl;
          StatusResult result = addBlock(block);
          stream.close();
          stream.clear();
//        if (result) std::clog << " Storage Constructor : success add block" << std::endl;
//        else std::clog << " Storage Constructor : failure add block" << std::endl;
        }
      } catch (...) {
        cout << " Storage Constructor : Cannot create new storage" << endl;
      }
    }

    // USE: ctor ---------------------------------------
    Storage::Storage(const std::string aName, OpenExistingStorage) : block(BlockType::meta_block), name(aName) {
      //try to OPEN a db file a given storage location
      //if it fails, throw an error
      const char* thePath = getDatabasePath(name).c_str();
      try {
        stream.open(thePath, fstream::in | fstream::out | fstream::binary);
        if (isReady()) {
          stream.seekg(stream.tellg(), stream.beg);
          StatusResult result = readBlock(block, 0);
        }
      } catch (...) {
        cout << "Cannot read database file" << endl;
      }
    }

    // USE: dtor ---------------------------------------
    Storage::~Storage() {
      stream.close();
    }

    // USE: validate we're open and ready ---------------------------------------
    bool Storage::isReady() const {
      return stream.is_open();
    }

    // USE: count blocks in file ---------------------------------------
    uint32_t Storage::getTotalBlockCount() {
      //how can we compute the total number of blocks in storage?
      stream.seekg(0, stream.end);
//    std::cout << stream.tellg() << " "  << (uint32_t) stream.tellg()  << " " << sizeof(StorageBlock) << " " << std::endl;
      return (uint32_t)stream.tellg() / sizeof(StorageBlock);
    }

    // Call this to locate a free block in this storage file.
    // If you can't find a free block, then append a new block and return its blocknumber
    StatusResult Storage::findFreeBlockNum() {
      StorageBlock curBlock;
      for (uint32_t i = 0; i < getTotalBlockCount(); ++i) {
        StatusResult readRes = readBlock(curBlock, i);
        if (readRes) {
          if (curBlock.header.type == 'F') {
            return StatusResult{Errors::noError, i};
          }
        }
      }
      return StatusResult{noError, getTotalBlockCount()}; //return blocknumber in the 'value' field...
    }

    // USE: for use with "storable API" [NOT REQUIRED -- but very useful]

    StatusResult Storage::save(Storable &aStorable) {
      //High-level IO: save a storable object (like a table row)...
      return StatusResult{noError};
    }

// USE: for use with "storable API" [NOT REQUIRED -- but very useful]

    StatusResult Storage::load(Storable &aStorable) {
      //high-level IO: load a storable object (like a table row)
      return StatusResult{noError};
    }

    // USE: write data a given block (after seek)
    StatusResult Storage::writeBlock(StorageBlock &aBlock, uint32_t aBlockNumber) {
      //STUDENT: Implement this; this is your low-level block IO...

      stream.seekg(stream.tellg(), stream.beg);
      size_t pos = aBlockNumber * sizeof(aBlock);
      stream.seekp(pos);
//    std::clog << " writeBlock : trying to write a block in " << aBlockNumber << "  " << pos << std::endl;
      try {
        stream.write((char*)&aBlock, sizeof(StorageBlock));
        this->cache.put(aBlock, aBlockNumber);
      } catch(...) {
        return StatusResult(Errors::writeError);
      }
      return StatusResult{};
    }

    // USE: read data from a given block (after seek)
    StatusResult Storage::readBlock(StorageBlock &aBlock, uint32_t aBlockNumber) {
      //STUDENT: Implement this; this is your low-level block IO...
      if (this->cache.containsBlock(aBlockNumber)) {
        aBlock = this->cache.get(aBlockNumber);
        return StatusResult{noError};
      } else {
        stream.seekg(aBlockNumber * sizeof(StorageBlock));
        if(stream.read ((char*)&aBlock, sizeof(StorageBlock))) {
          return StatusResult{noError};
        }
      }
      return StatusResult{readError};
    }

    const std::string Storage::getDBPath(const std::string &aName) {
      std::ostringstream out;
      StorageInfo storageInfo;
      out << storageInfo.getDefaultStoragePath() << "/" << aName << ".db";
      return out.str();
    }


    StorageBlock& LRUCache::get(uint32_t blockNum) {
      Node* cur = map.at(blockNum);
      removeKey(blockNum);
      addHead(cur);
      return cur->block;
    }

    void LRUCache::put(StorageBlock& block, uint32_t blockNum) {
      Node* newNode = new Node(blockNum, block);
      if (map.find(blockNum) != map.end()) {
        removeKey(blockNum);
      }
      addHead(newNode);
    }

    bool LRUCache::containsBlock(uint32_t blockNum) {
      return map.find(blockNum) != map.end();
    }

}
