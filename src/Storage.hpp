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
#include <string>
#include <map>
#include <fstream>
#include <variant>
#include <sstream>
#include <unordered_map>
#include "Errors.hpp"
#include "StorageBlock.hpp"
#include "Value.hpp"

using namespace std;

namespace ECE141 {

    //first, some utility classes...

    class StorageInfo {
    public:
        static const char* getDefaultStoragePath();
    };

    struct CreateNewStorage {};
    struct OpenExistingStorage {};

    class Node {
    public:
        Node(uint32_t blockNum = 0, const StorageBlock &block = StorageBlock(), Node *prev = NULL, Node *next = NULL) : blockNum(blockNum), block(block),
                                                                                                                        prev(prev), next(next) {}
        uint32_t blockNum;    // key
        StorageBlock block;   // val
        Node *prev;
        Node *next;
    };

    class LRUCache {
    public:
        LRUCache(int capacity = 64, int size = 0, Node *head = new Node(), Node *tail = new Node()) : capacity(capacity), size(size), head(head),
                                                                                                      tail(tail) {
          head->next = tail;
          tail->prev = head;
        }
        StorageBlock& get(uint32_t blockNum);
        void put(StorageBlock& block, uint32_t blockNum);
        bool containsBlock(uint32_t blockNum);

    protected:
        int capacity;
        int size;
        Node* head;
        Node* tail;
        unordered_map<uint32_t, Node*> map;

        void removeKey(uint32_t key) {
          Node* cur = map[key];
          Node* prev = cur->prev;
          Node* next = cur->next;
          prev->next = next;
          next->prev = prev;
          map.erase(key);
          size--;
        }

        void addHead(Node* node) {
          Node* next = head->next;
          head->next = node;
          node->next = next;
          next->prev = node;
          node->prev = head;
          map[node->blockNum] = node;
          size++;
          if (size > capacity) {
            Node* last = tail->prev;
            removeKey(last->blockNum);
          }
        }
    };


    // USE: Our storage manager class...
    class Storage {
    public:

        Storage(const std::string aName, CreateNewStorage);
        Storage(const std::string aName, OpenExistingStorage);
        ~Storage();
        uint32_t        getTotalBlockCount();

        //high-level IO (you're not required to use this, but it may help)...
        StatusResult    save(Storable &aStorable); //using a stream api
        StatusResult    load(Storable &aStorable); //using a stream api

        StatusResult    findFreeBlockNum();

        //low-level IO...
        StatusResult    readBlock(StorageBlock &aBlock, uint32_t aBlockNumber);
        StatusResult    writeBlock(StorageBlock &aBlock, uint32_t aBlockNumber);

        static const std::string getDBPath(const std::string &aName);

        StatusResult    addBlock(StorageBlock& aBlock);
        StatusResult    addBlock(StorageBlock& aBlock, string tableName);
        LRUCache        cache;

    protected:
        bool            isReady() const;

        StorageBlock    block;
        std::string     name;

        std::fstream    stream;
    };

}

#endif /* Storage_hpp */
