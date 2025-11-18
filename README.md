Simple File Based Database Engine in C

This is a simple database engine which stores data in separate TXT files (for now, in future we will use .bin files).

Data types available: int (integers), str(strings), bool(boolean)
You can set the data type and length of each field during the creation of the database

The following commands are working properly:

- OPEN: Select a database file to work on
- CREATE: Create a new database file
- OVERVIEW: Show list of all data inside a db
- INSERT: Insert new data in the database
- ROWDELETE: Delete a particular  row of data 
- FIELDDELETE: Delete a field
- RENAME: Rename a field
- EDIT: Edit a particular cell's data given its row number and field name
- ADDFIELD: Add a new field to the database
- COMMIT: Commit all changes to the database file permanently
- HELP: Show list of all commands and how to use them

Commands and features yet to come:
- Select statments to show particular data
- Primary key system