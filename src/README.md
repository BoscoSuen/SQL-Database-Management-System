# SP20-Assignment2 
#### Due Tuesday Apr 14, 2020 @ 11:15pm

## Getting started with our database.

When you DB application is running, it can operate in two modes. The first is interactive mode, and the second is script mode. Interactive mode is where the program waits for you to provide input and then processees it. This continues until you type the `quit` command. The second mode is "script" mode. Script mode runs when you run your application from the command line, and pass a script as the first argument. In this mode, the program tries to run the given script, and then quits automatically.

In this assignment, we're going to get our first two command processors setup and ready to handle commands. For our application processor, we will actually implement three commands (help, version, quit). For our DB command processor, we are just getting things ready to be dealt with in our next assignment. The DB command processor won't actually run commands just yet.

## General note about command handling 

### Handling Commands 

As we discussed in lecture, command handling goes through a specific series of steps:

1. the user inputs a command (or we read it from a script file)
2. we tokenize the input
3. we use available "command processors" (in a chain-of-responsibility) to find a processor that thinks it can handle the command by calling `processInput(tokenizer)`
4. the processor that knows the command will instantiate a command-specific custom `Statement` object, and initialize it with data found in the tokenizer
5. next, the processor will call its own method, `interpret(statement)` to dispatch the command. This method is a router, whose job is dispatch the statement. We will dispatch statements to custom code (a method) in your command processor that knows how to handle statements of that type. There are times, however, that you may call a method in a different processor. That design decision is up to you.

You'll notice that we provided a default version of your `AppProcessor` class, a sub-clsas of the `CommandProcessor` interface. This is a good example of how to get started building your own command processing class.  This is where you write code to do the work we've outlined above in the "Handling Commands" section -- for the application level commands. Other commands will be handled in other command processors.

### Custom Statement Subclasses

For every command we support (in this and future assignments), you need to create a custom subclass of `Statement`.  This class will contain all the relevant information given by user input (in the tokenizer) necessary to run a specific command. Some commands don't require arguments, and there associated `Statement` classes are trivial. Some commands take many complex arguments and will take more work on your part.  Ultimately, you'll have many custom statement classes, like this:

```
VersionStatement
HelpStatement
QuitStatement
CreateDBStatement
DropDBStatement
UseDBStatement
DescribeDBStatement 
ShowDBsStatement
//etc....
```

You're free to build your `Statement` sub-classes any way you want. You might consider grouping your "application statement" classes together in one file, and your "db statement" classes in a another file. Or you might just make a separate file for each `Statement` sub-class.

## Part 1 - Application Command Processor 


### Prepare to handle the following application commands 

1. help command
2. version command
3. quit command

For each of the following commands, you need to do the following things:

1. Build a custom sub-class of `Statement`
2. In your `Processor::getStatement()` method, construct the type of `Statement` (subclass) associated with given command 
3. In your `Processor::interpret(Statement)`, determine which method on your `Processor` class to call, and then call that method to perform the work associated with the given `Statement`.  

If errors occur, make sure to report then to the user on `std::cout`. 

#### `help` Command

The help command is used to output command-related information to the user. If the user enters the keyword `help` by itself, your code should output a synopsys of all the commands it knows about so far.  

```
help      //user types the help command 
help -- the available list of commands shown below:
     -- help //put your explanation here ...
     -- version //explanation here... 
     -- quit    //etc.. 
     -- create database <name>    //etc.. 
     -- drop database <name>    //etc.. 
     -- use database <name>    //etc.. 
     -- describe database <name>    //etc.. 
     -- show databases     //etc.. 
```    

Example output of help command can be 

```
help      
help -- the available list of commands shown below:
     -- help - shows this list of commands
     -- version -- shows the current version of this application
     -- quit  -- terminates the execution of this DB application 
     -- create database <name> -- creates a new database  
     -- drop database <name> -- drops the given database
     -- use database <name>  -- uses the given database 
     -- describe database <name>  -- describes the given database
     -- show databases   -- shows the list of databases available
```    
     
If the user enters the `help` keyword followed by a the name of a command (shown below), then your program should output detailed information about using that specific command:

```
help select   //then output information about the select command
```

#### `version` Command

The version command is used to inform the user about the version number of the applictation. For this assignment, you will output the value of 'ECE141b-1'. Next week, you'll output the value of 'ECE141b-2', and continue to increment the version number each week. Your output should look like this:

```
version              //user typed this command
version ECE141b-1    //your program outputs this (without this command)
```

#### `quit` Command

Assuming that your program is running in "interactive mode", this command will cause your application to exit.


## Part 2 - Database Command Processor

This command processor is a bit more challenging than the "application command processor". Database commands actually change the environment. They can cause files to be created and destroyed. You have to create and implement your own class to handle database level commands. Like the given `AppProcessor` class, your db command processor will be a subclass of `CommandProcessor`. 

> One thing you need to think about is where you are going to store the database files your user asks you to create. We've included a class called `StorageInfo` in the `Storage.hpp` file (very minimal for now), that has a static method called `getDefaultStoragePath`. Make sure you set up a valid path in that function to a location on your machine where you can store DB files.


### Prepare to handle the following DB commands

1. create database <name> 
2. drop database <name>
3. use database <name>
4. describe database <name>
5. show databases 
  
#### `create database <name>`

This command will cause a new database file to be created. For now, that file can be a simple text file with the given name, but soon it will be a binary-encode file that contains all the information related the tables for a given database. 
For example, if you are trying to create a new database (file) using the command `create database new_db` , the output is shown below
```
> create database new_db
created database new_db (ok)
```

#### `drop database <name>`

When this command is executed, the database with the given name will be deleted from storage. If that DB is currently in use by your user, it needs to be closed and removed from memory (no longer in use). 
For example, if you are trying to drop a created database (new_db) using the command `drop database new_db`, the output is shown below
```
> drop database new_db
dropped database new_db (ok)
```

#### `use database <name>`

This command sets state in your current interpreted environemnt, that the named database is the target that will recieve table-oriented commands, like creating, inserting, updating, and deleting records, as well as creating and deleting tables themselves. 
For example, if you are trying to use a created database using the command `use database new_db_1`, the output is shown below 
```
> use database new_db_1
using database new_db_1
```

#### `show databases`

When invoked, this command needs show (on the terminal output) the names of all database files that have been created (and not deleted) by your user. 
For example, if you have created a database `new_db` and `new_db_1` the command `show databases` gives an output as shown below.

```
> show databases
new_db
new_db_1
```

#### `describe database <name>`

> This is a non-standard command, that we use for testing

This command is used to print out information about the structure and contents of the named database. For now, this command doesn't have to do any work. 


## Part 3 - Folder Reader/View

In preparation for our next assignment, we're going to get started on a two utility classes. `FolderReader` and `FolderView`. 

### FolderReader 

The `FolderReader` class is responsible for reading files in a directory, and reporting them back to an observer class using the `FolderListener` interface.  (See both of these in the `FolderReader.hpp` file).  We have provided a starting version of this class. You are required to complete the implementation of this class. 

Use the `FolderReader` class to retrieve and iterate the files in a given folder.  Your task is to implement `FolderReader.each(Listener)` so that for any given folder, it calls the given `Listener` with each file it finds in folder. 

To test this class, you'll make a test class that implements the `FolderListener` interface (1 method), and show that your `FolderReader` calls your `FolderListener` for each file is a given directory.

## Example Output 

It's important for every command you run, that your program output information that makes it clear what command it is running, along with the results of the command.

Let's say that someone runs your application, and types the command "version" in your interactive console.  Your program will determine how to handle this command, and will attempt to build an associated `Statement`, and attempt to exectute that statement.  The output (in your console using std::clog) will look like this:

```
> version   //user typed the version statement; then you attempt to run it...
version 0.1   //your program makes it clear it's running the version command, and offers the current version
```

> All normal program output in your program should be sent out using the `std::cout` stream.  Any debugging output you want to view should be written to the `std::clog` stream. 

## Testing

It's important to test your program two ways. First, test in "interactive mode" when your program is running by typing commands into the program terminal window. Second, test by running your program on the command line, with an argument that specifies a test script. Test scripts are just text files that contain commands in the same format as you would type in interactive mode. For example, you could create a test file to test the "version" command as shown below. Note that a test script file can contain more than 1 command, separated by a semicolon.

(contents of test.txt)
```
version
```
A few things you have to make sure your code follows is given below: 
* Database naming rules
For now, please make sure you follow the below rules when naming the database files: 
1. Names can contan only alphanumeric characters and must begin with an alphabetic character.
2. Database names cannot begin with an underscore
3. Names can contain the following special characters: 0 through 9, #, @ and $ but must not begin with these characters.

* Output expectations
1. For all the commands, make sure your output matching a similar output as given in the examples. You can double check these by seeing the failed test cases.
2. Be wary of the corner case, you should return appropriate error codes for these cases. 

* Autograding with Github classroom 
We will be testing your code with a GitHub based autograder. This [resource](https://classroom.github.com/help/reviewing-auto-graded-work-students) gives a detailed explanation on what to expect and where to look for results. 

## Grading 

### A note about output from your program

In all the of coding assignmnts from ECE141b -- your work will be graded based on the output your program produces when provided a set of input. It is absolutely essential that you follow a few basic rules for your output. 

1. For debug output for your own purposes, use `std::clog` (NOT `std::cout`) to write information to your terminal. 
2. For "graded" program output, use `std::cout`.  Our testing framework will catch this output, and compare it to the expected output

Our tests will all be based on testing scripts that we send to your application, so make sure you include tests of this type too. 

## Sumbitting Your Work -  Due Tuesday Apr 14, 2020 @ 11:15pm

You will ONLY submit your work via github classroom. Make sure that your `students.json` file contains the names of all the members in your team (1-2 people max). 


