In order for ROOT to access to the library we just created you must do: 
```
export ROOT_INCLUDE_PATH=/home/fran/restFileFilter/inc:$ROOT_INCLUDE_PATH
export LD_LIBRARY_PATH=/home/fran/restFileFilter/build:$LD_LIBRARY_PATH
```
and for libpq-fe: 

```
export ROOT_INCLUDE_PATH=/usr/include/postgresql:/home/fran/restFileFilter/inc
export LD_LIBRARY_PATH=/usr/lib/x86_64-linux-gnu:/home/fran/restFileFilter/build
``