#ifndef POSTGRESDB_H
#define POSTGRESDB_H

#include <string>
#include <libpq-fe.h>
#include <vector>
#include <iostream>


class PostgresDB {
public:
    PostgresDB(const std::string& connectionString);
    ~PostgresDB();

    void executeQuery(const std::string& query);
    void filteringQuery(const std::string& table, const std::string& column, const std::string& value);
    void showTables();
    void showColumns(const std::string& table);
    void showValues(const std::string& table, const std::string& column);

private:
    PGconn* fConnection;
};;

#endif // POSTGRESDB_H
