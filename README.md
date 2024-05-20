# File System Indexer

File System Indexer for the REST Web Explorer

This program should always be running in the server whose filesystem we want to index. To prevent a potentially
catastrophic failure (such as deleting the whole filesystem) this program should only be run with read-only privilege
levels, since there is no need to modify files.

### Dependencies

If possible, run this software from a Docker container, as it's the easiest way to run the code.
If this is not possible, you will need to install several dependencies.

The program is writen in C++ and uses ROOT and REST libraries to extract useful information from REST (.root) files and
index them in a database system.

To compile this program you need a ROOT installation compiled with C++17. You can check the compilation flags of your
ROOT installation via `root-config --cflags`.

The connection to the database is done via ROOT's `TSQLServer` class. To connect to the database it is necessary to have
the appropiate plug-ins installed (postgresql).
`TSQLServer` won't know how to talk to the database if the corresponding database libraries weren't installed at the
time of building ROOT.

To enable compatibility with postgresql databases you would need to install (on Ubuntu):

```
apt install -y libpq-dev postgresql-server-dev-all
```

Then build root the usual way, and the libraries should be installed automatically. There is no need to specify
additional build flags.

A quick way to check if the corresponding plug-in is installed in your ROOT installation is the following:

```
   ------------------------------------------------------------------
  | Welcome to ROOT 6.25/01                        https://root.cern |
  | (c) 1995-2021, The ROOT Team; conception: R. Brun, F. Rademakers |
  | Built for linuxx8664gcc on Jun 21 2021, 12:29:00                 |
  | From tags/v6-25-01@v6-25-01                                      |
  | With                                                             |
  | Try '.help', '.demo', '.license', '.credits', '.quit'/'.q'       |
   ------------------------------------------------------------------

root [0] gROOT->GetPluginManager()->FindHandler("TSQLServer", "pgsql://localhost:5432/postgres")->CheckPlugin()
(int) 0
```

If the result is `0` it means the plug-in is correct installed. Otherwise it will return `-1`.

To connect to the database (using the corresponding credentials):

```
TSQLServer *db = TSQLServer::Connect("pgsql://localhost:5432/postgres", "postgres", "password");
```

The program also uses some non-standard dependencies such as [inotify-cpp](https://github.com/erikzenker/inotify-cpp), a c++ wrapper for `inotify` which itself requiered `BOOST` libraries.

To install this library follow the instructions in the [GitHub](https://github.com/erikzenker/inotify-cpp). To install BOOST (in Ubuntu):

```
apt install -y libboost-all-dev
```
