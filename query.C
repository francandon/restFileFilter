// query.C
#include "PostgresDB.h"
#include <string>

// Function to be called from the ROOT macro
void query(int fRunNumber) {
    std::string conninfo = "host=localhost port=5432 dbname=postgres user=postgres password=password";
    PostgresDB db(conninfo);

    std::string query = "SELECT * FROM ONLY \"TRestRun\" WHERE \"fRunNumber\" = " + std::to_string(fRunNumber);
    db.executeQuery(query);
}