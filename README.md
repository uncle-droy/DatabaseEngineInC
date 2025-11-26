# Simple File Based Database Engine in C

This is a simple database engine which stores data in separate TXT files (for now, in future we will use .bin files).

Data types available: int (integers), str(strings), bool(boolean)
You can set the data type and length of each field during the creation of the database

The following commands are working properly:

- OPEN <FILEPATH>: Select a database file to work on
- CREATE: Create a new database file
- OVERVIEW: Show list of all data inside a db
- INSERT <VALUES>: Insert new data in the database
- ROWDELETE <ROW_NUMBER>: Delete a particular  row of data 
- FIELDDELETE <FIELD_NAME>: Delete a field
- RENAME <OLD_FIELD_NAME> <NEW_FIELD_NAME>: Rename a field
- EDIT <ROW_NUMBER> <FIELD_NAME> <NEW_VALUE>: Edit a particular cell's data given its row number and field name
- ADDFIELD <FIELD_DEFINITION>de: Add a new field to the database
- COMMIT: Commit all changes to the database file permanently
- HELP: Show list of all commands and how to use them

Commands and features yet to come:
- Select statments to show particular data
- Primary key system