#include "PostgresDB.h"
#include <iostream>

PostgresDB::PostgresDB(const std::string &connectionString) {
    fConnection = PQconnectdb(connectionString.c_str());
    if (PQstatus(fConnection) != CONNECTION_OK) {
        std::cerr << "Connection to database failed: " << PQerrorMessage(fConnection) << std::endl;
        PQfinish(fConnection);
        fConnection = nullptr;
    }
}

PostgresDB::~PostgresDB() {
    if (fConnection) {
        PQfinish(fConnection);
    }
}

void PostgresDB::executeQuery(const std::string &query) {
    if (!fConnection) {
        std::cerr << "No active database connection" << std::endl;
        return;
    }

    PGresult *res = PQexec(fConnection, query.c_str());

    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
        std::cerr << "Query failed: " << PQerrorMessage(fConnection) << std::endl;
        PQclear(res);
        return;
    }

    int nFields = PQnfields(res);
    int nRows = PQntuples(res);

    for (int i = 0; i < nFields; i++) {
        std::cout << PQfname(res, i) << "\t";
    }
    std::cout << std::endl;
    std::cout << std::endl;
    for (int i = 0; i < nRows; i++) {
        for (int j = 0; j < nFields; j++) {
            std::cout << PQgetvalue(res, i, j) << "\t";
        }
        std::cout << std::endl;
        std::cout << std::endl;
    }
    PQclear(res);
}

void PostgresDB::filteringQuery(const std:: string &table,const  std::string &column,const std::string &value) {
    std::cout << "**************FILTERING QUERY RESULTS ARE:****************" << std::endl;
    std::string query = "SELECT \"path\" FROM ONLY \""+ table +"\" WHERE \""+ column +"\" = " + value;
    executeQuery(query);
}
void PostgresDB::showTables() {
    std::string query = "SELECT table_name FROM information_schema.tables WHERE table_schema = 'public'";
    std::cout << "**************DATABASE TABLES ARE:****************" << std::endl;
    executeQuery(query);
}

void PostgresDB::showColumns(const std::string &table) {
    std::string query = "SELECT column_name FROM information_schema.columns WHERE table_name = '" + table + "'";
    std::cout << "**************" + table + "COLUMNS ARE:****************" << std::endl;
    executeQuery(query);
}
void PostgresDB::showValues(const std::string &table, const std::string &column) {
    std::string query = "SELECT DISTINCT \"" + column + "\" FROM \"" + table + "\"";
    std::cout << "**************"+ column+ "VALUES FROM TABLE " + table + " ARE: ****************" << std::endl;
    executeQuery(query);
}