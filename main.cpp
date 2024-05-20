#include "PostgresDB.h"
#include <string>
#include <vector>

int main() {
    std::string conninfo = "host=localhost port=5432 dbname=postgres user=postgres password=password";
    PostgresDB db(conninfo);
    db.showTables();
    db.showColumns("files");
    db.showValues("TRestRun", "fEndTime");
    db.filteringQuery("TRestRun", "fRunNumber", "1664");
    return 0;
}