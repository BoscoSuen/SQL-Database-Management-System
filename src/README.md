# ECE141-SP20-Assignment8
### Due Friday - May 29, 2020 - 11:15pm 

Building an index to improve performance

## Overview -- Working with an Index

In this assignment, we are going to add a primary-key index to tables that we create.  We will update the index as we add/update/delete rows in the table. And we will make our index persistent, so it's available whenever we open a `Database` to work with data rows.

<hr>

## Key Classes in This Assignment 

You'll discover that the following classes are significant in this assignment:

### The `Timer` class

This is a simple, high-speed timer that we will use to measure the performance of our `SELECT` queries. These measurements will validate whether our performance has improved using indexes.

### The `BlockIterator` class

Our database already offers a `BlockVisitor` class, for use when you call `Storage::each()` and want to visit each block in storage. This was necessary, for example, to iterate blocks and find those associated with a given table. Now that we have an index, we can avoid iterating all blocks via `Storage`, and instead, iterate blocks from the `Index`.   Therefore, we want both classes to offer a consistent, interface to walk over available blocks.

### The `Index` class

The `Index` class will provide a map between a specified table-field, and a block number:

```
std::map<KeyType, uint32_t> data;
```

We have provided a minimal version of this class for your use. You are free to use this, or change it in any way you see fit.  

<hr>

## Integrating files from assignment #7 with this assignment

To do this assignment, you will copy all your files from assignment-#7 project folder into this folder. You don't have to worry about overwriting anything, as there are no files in this folder that were also in previous assignments.

<hr>

## Adding support for the `Timer` class

For this assignment you will integrate the provided `Timer` class into the code where you run `SELECT` queries.

The `Timer` class has three key methods you'll want to use:

1. `Timer.start()` -- which starts the timer
2. `Timer.stop()` -- which stops the timer
3. `Timer.elapsed()` -- which provides the elapsed time between start/stop events

### Timing `SELECT` Statements 

In your `select query` code, you will `start()` your timer *BEFORE* building your collection of rows. Do not include the time it takes to parse the `SELECT` statement. 

You will `stop()` your timer after your rows are assembled, but *BEFORE* you show your results in a view. Here's an example of what that output looks like:

```
> select * from users;
+---------+--------------------+--------------------+
| id      | firstname          | lastname           | 
+---------+--------------------+--------------------+
| 2       | megan              | gessner            | 
+---------+--------------------+--------------------+
1 rows in set (0.000108458 ms.)
```

### Timing `UPDATE` Statements 

In your `UPDATE` code, you will `start()` your timer *BEFORE* building your collection of rows. Do not include the time it takes to parse the `UPDATE` statement. You will `stop()` your timer *AFTER* the associated rows have been changed in the storage file.  Your output for the `UPDATE` statement should include the timing result -- and look like this:

```
> UPDATE users SET firstname='Megan' where id=2
1 rows affected (0.0001032 ms.)
```

### Timing `DELETE` Statements 

In your `DELETE` code, you will `start()` your timer *BEFORE* building your collection of rows. Do not include the time it takes to parse the `DELETE` statement. You will `stop()` your timer *AFTER* the associated rows have been deleted from storage.  Your output for the `DELETE` statement should include the timing result -- and look like this:

```
> DELETE from users where id=2
1 rows affected (0.0001032 ms.)
```

<hr>

## Adding support for the `BlockIterator` class

The `BlockIterator` (interface) is shown below. It's exactly the same as the `Storage.each()` function you're already using. We separate this into distinct interface so that we can let the `Index` class provide the same interface for iterating blocks. 

```
 struct BlockIterator {
    virtual bool each(BlockVisitor &aVisitor)=0;
    virtual bool canIndexBy(const std::string &aField) {return false;} //override this
 };
```

To get this to work, do the following steps:

1. manually add the `BlockIterator` class to storage.hpp, after the `BlockVisitor` class
2. make the `Storage` class a subclass of `BlockIterator`

You'll notice that the simple `Index` class we provided already implements this interface. 

<hr>

## Defining your `Index` class

We've provide an outline for your `Index` class. It's up to you to adapt this code to meet your needs. You can change it any way you like, or replace all of it with your own.

The `Index` class has three main responsibilities:

### 1. It stores key/value pairs to quickly find row blocks in storage

The main job of the `Index` class is to hold a set of key/value pairs that map a field (in every row of a table) with the blocknumber where that row was located in your `Storage` DB file. Therefore, adding, updating, removing, and finding block number (values) based on a field (key) are essential methods.  We provided a basic set of methods for this purpose. Feel free to change them:

```
- addkey/value
- find key/value
- remove key/value
- getvalue(key)
- bool contains(key)
```

### 2. And `Index` can read/write itself (either in standalone files, or in storage)

In the version we provide, the `Index` class is `Storable`. You don't have to use that approach, for example, if you prefer to read/write index files as standalone files. 

### 3. It offers the `BlockIterator::each()` method to iterate blocks assocated with data rows in the table

When you call the `Index.each()` method, it will call your visitor object one time for each value in the index. This will allow you to read/search/use associated rows without having to scan every block in a `Storage` DB file.


## Adding Support For the `Index` Class

Your system will interact with `Index` classes in the following circumstances:

### `CREATE` Table

When you `CREATE` a table, you will automatically create an index associated for the primary key of the table.

### `DROP` Table

When you `DROP` a table, you will automatically delete all the associated indexes and their associated data.

### `INSERT` Rows

When you `INSERT` row, you will add an entry (key field) from the row into each table index.

### `SELECT` Rows

When you 'SELECT" rows, you try to locate the associated rows by searching your indexes. This is naturally dependent on the nature of the query, and whether an appropriate index is available.  Remember -- in the worst case (where no index exists matches the conditions specified in your `WHERE` clause) -- you can always use the primary key index to load records for your table, rather than scanning all block in a storage file.

### `UPDATE` Rows

When you 'UPDATE" rows, you try to locate the associated rows by searching your indexes. (See `SELECT` above). If the `UPDATE` statement specifies changes to fields that are used as the key in an index, the associated index value must be updated. 

### `DELETE` Rows

When you 'DELETE" rows, you try to locate the associated rows by searching your indexes. (See `SELECT` above). Before you delete the associated rows, make sure to remove the key (field) for each from from your associated indexes.

## Implement the `SHOW INDEXES` Command

The last step in this assignment is to add support for the `SHOW INDEXES` command, that shows all the indexes defined for a given database:

```
> show indexes
+-----------------+-----------------+
| table           | field           | 
+-----------------+-----------------+
| users           | id              |  
+-----------------+-----------------+
1 rows in set 
```


## Testing This Assignment

### Performance 

Currently, when you run a `SELECT` statement, you're forced to scan all the blocks in your storage file. With the introduction of indexing, we don't have to scan all the storage blocks. If your `WHERE` clause refers to a field that is indexed, we can scan the index in memory to find associated blocks to achieve the best performance. If your `WHERE` clause refers to fields that are not indexed, we can still use our primary key index to reduce the number of blocks from "all blocks in storage" to just data blocks for rows associated with this table.   

Regardless of which scenario you encounter, having an index should improve your performance. We can see the performance improvement in the summary line of your table row, where we include timer output (shown below). Given a table with a susfficient number of records, our auto-grader will run your system through a scenario where we query on indexed vs non-indexed fields, so that we can detect a performance differential.

```
> select * from users;
+---------+--------------------+--------------------+
| id      | firstname          | lastname           | 
+---------+--------------------+--------------------+
| 2       | megan              | gessner            | 
+---------+--------------------+--------------------+
1 rows in set (0.000108458 ms.)
```

As always you can use the auto-grader to help get your code to perform as expected.


## Turning in your work 

Make sure your code compiles, and meets the requirements given above. Also make sure you updated your students.json file with the name of each contributor.  Submissions that don't include a completed `students.json` file will receive 0.

Submit your work by checking it into git by <b>Friday - May 29, 2020 - 11:15pm </b>. Good luck! 
