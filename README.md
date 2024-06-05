# restFileFilter
This C++ program connects to a postgreSQL database and allows the user to make queries in a simplified way. 

## Connecting to the data base 
In order to connect to the database, one must call the default constructor passing to it the credentials of the database: 
```
std::string conninfo = "host=localhost port=5432 dbname=postgres user=postgres password=password";
PostgresDB db(conninfo);
```
## Making Queries 
Rigth now, there are 4 methods that allows you to make simple queries:
- void filteringQuery(const std::string& table, const std::string& column, const std::string& value)   -> Prints you the paths of the files that corresponds to  a certain value of a column table.
- void showTables()  -> Plots the tables availables in the database.
- void showColumns(const std::string& table)  -> Plots the columnds available in a table.
- void showValues(const std::string& table, const std::string& column)   -> Shows all the values of a certain table column 
## Example
In this example we connect to the database, then make some queries. For example we obtain the files from TRestRun which fRunNumber is 1664:
```
std::string conninfo = "host=localhost port=5432 dbname=postgres user=postgres password=password";
PostgresDB db(conninfo);
db.showTables();
db.showColumns("files");
db.showValues("TRestRun", "fEndTime");
db.filteringQuery("TRestRun", "fRunNumber", "1664");
```
## Compilation
In order for ROOT to access to the library we just created you must type on the terminal, or place it on your bashrc: 
```
export ROOT_INCLUDE_PATH=/home/fran/restFileFilter/inc:$ROOT_INCLUDE_PATH
export LD_LIBRARY_PATH=/home/fran/restFileFilter/build:$LD_LIBRARY_PATH
```
and for libpq-fe: 

```
export ROOT_INCLUDE_PATH=/usr/include/postgresql:/home/fran/restFileFilter/inc
export LD_LIBRARY_PATH=/usr/lib/x86_64-linux-gnu:/home/fran/restFileFilter/build
```
For compiling and executing just go to the folder build and make:
```
cmake ..
make
./main
```

