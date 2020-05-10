# ece141-sp20-assignment3
### Due - Tuesday, April 21 at 11:15pm 

## Building Your Storage Sub-system

In this assignment, you will develop a basic storage engine for your database system. Builting upon that, you will implement the first working versions of the following commands:

1. "create database `name`" 
2. "describe database `name`" 
3. "drop database `name`" 


## Overview 

The `Database` class is an object that represents an open database file. This is the class that will offer methods that actually do the work when a database related command is issued by the user, such as:

1. create database `name`
2. drop database `name`
3. create table `name` ... 
4. insert into `name`...

To recap from lecture: the `Database` object works with in-memory, database-related concepts, whereas the `Storage` objects works in terms of fixed-sized block I/O to manage DB files. Much of the code you write will be to translate from 'database related objects' to "storageblocks" and back again.

### About the `Database` class 

When your user asks to `use database foo`, you will create a `Database` object. `Database` objects work with other logical database-related objects, such as `Rows`, `Schemas`, `RowCollections`, `Indexes` and so on.  These are all high-level objects (models). These are also all "in-memory" representations of these models.  Your `Database` object will also contain/use a `Storage` object.  

### About The `Storage` class

A `Storage` object performs all the file-related I/O operations for the system. The `Storage` object builds and maintains DB (storage) files. The `Storage` object operates in terms of reading and writing "StorageBlocks".  A `StorageBlock` is a fixed sized data structure, that compreise each of the elements inside a DB (storage) file.   

### Understanding DB (storage) files 

A database file is a sequence of fixed-sized data blocks. The default blocksize if 1024 bytes but you are free to change this.  Each block has two parts. First is a small block header that includes meta information, such as the block-type (see this in `Storage.hpp`). The header if followed by a data buffer, that contains your data.  It looks something like this:

```
[ META   | ............................... ]
[ SCHEMA | ............................... ]   
[ DATA   | ............................... ]   
[ DATA   | ............................... ]   
etc.
```

#### Random Accessibiilty 

Having fixed-sized blocks has another important benefit. It allows your `Storage` engine to move randomly to any block in the file -- and quickly access any data as needed.

#### Multi-part blocks

In most cases, elements you write into a storage file will fit into a single block. There are a few cases, however, where your data won't fit in a single block, and you need to group some blocks together. For example, when we get to building indexes, it's quite possible that an index won't fit in a single block.  Handling that issue is left up to you. However, you might consider how storing information in the block header might help you identify and coordinate this case.

<hr> 

## Integrating files from assignment #2 with this assignment

You'll notice the this assignment contains some new files, and 3 files that were part of Assignment#2.  To do this assignment, you will copy your files from assignment#2 project folder into this folder.   DO NOT to overwrite the updated files we provided in this assignment. Only copy files for your assignment#2 that aren't already here. 

There are four files in this assignent that also existed in assignment #2. Do not overwrite these files:

- makefile
- Storage.hpp
- Storage.cpp 
- students.json

### A small change for portability 

In the original version of `StorageInfo::getDefaultStoragePath()` you provided a path to the location on your machine where you want to store your database files. In order to allow for greater flexibility, we are going to update this method to read the value for this path from your environment.  To read the environment variable, you can use the system function `getenv(name)`.  Setting an environment variable is an OS specific process. Most IDE's let you do this within the IDE to make debugging easier.  

<hr> 

## Part 1 - Implement "create database `name`" 
#### Estimated Time: 6 hours

In assignment #2, our goal was to connect user input to our command handlers, but not actually create a real databae file. Now we will do the work to produce an actual database file. 

As we discussed in lecture, a database file contains a sequence of fixed sized blocks. When the "create database" command is issued, we need to construct of a new file with a given name.  A new `Database` object is created, and works with the Storage object to complete this task.  Key to this assignment is to add functionality to our `Database` and `Storage` objects to achieve this outcome.

##### -- Add  the method `createDatabase(std::string &aName)` to the `Database` class

Call this method when the user issues a "create database ..." command. When called, this method will attempt to create a new database file (with the given name). A new database file will contain one block (index=0) - that has a header type of 'Meta'. 

Your `Database` object will use the `Storage` object `writeBlock` method to cause the first "meta" block to be appended to the database file. 

<hr> 

## Part 2 - Implement "describe database `name`"
#### Estimated Time: 3 hours

##### -- Add  the method `describeDatabase(std::string &aName)` to the `Database` class

Call this method when the user issues a "describe database `name`" command. When called, this method will iterate all the blocks in the named datatabase file, and print a summary to std::cout.  For example, lets assume we added table called "users", and add two records, the output would look like this:

```
Blk#  Type    Other
----------------------------
0     Meta 
1     Schema  "users"
2     Data    id 1
3     Data    id 2 
```

<hr> 

## Part 3 - Implement "drop database `name`" 
#### Estimated Time: 30 minutes 

##### -- Add  the method `dropDatabase(std::string &aName)` to the `Database` class

When the user issues the "drop database `name`" -- call the Database.dropDatabase(name) method. The result of this method is to actually delete the given database file. If that DB is currently in use by your user, it needs to be closed and removed from memory (no longer in use). 

<hr>

## Grading

Grading for this assignment will be based your system's ability to create, describe, and drop valid database files. You may submit multiple times to github classroom autograder to see your automated grading results. 

- code compiles and runs: 30 points
- create valid file: 30 points
- describe valid file: 30 points 
- drop valid file:  10 points 

## Submitting Your Work
### Due - Tuesday, April 21 at 11:15pm 

Make sure you submit your work (individually or as a team). Also make sure your students.json file contains information about each student who worked on your assignment. 





