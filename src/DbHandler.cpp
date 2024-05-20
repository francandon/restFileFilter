#include "DbHandler.h"

#include <TCollection.h>

#include <fstream>
#include <iostream>
#include <set>
#include <stdexcept>
#include <string>
#include <vector>
#include <utility> // For std::pair
#include <filesystem>

using namespace std;
namespace fs = std::filesystem;

DbHandler* DbHandler::pinstance_ = nullptr;

DbHandler* DbHandler::Instance() {
    if (pinstance_ == nullptr) {
        pinstance_ = new DbHandler();
    }
    return pinstance_;
}

DbHandler::DbHandler()
    : schema(std::getenv("DATABASE_SCHEMA") ?: "pgsql"),
      host(std::getenv("DATABASE_HOST") ?: "localhost"),
      port(std::getenv("DATABASE_PORT") ?: "5432"),
      database(std::getenv("DATABASE_NAME") ?: "postgres"),
      user(std::getenv("DATABASE_USER") ?: "postgres"),
      password(std::getenv("DATABASE_PASSWORD") ?: "password"),

      scriptsDir("database/scripts") {
    cout << scriptsDir << endl;
    cout << "Initializing database handler..." << endl;
    db = connect();
    cout << "Connection to database " << (db ? "OK" : "FAILED") << endl;

    if (!db) {
        throw std::runtime_error("Error connecting to database");
        return;
    }

    cout << "Loading scripts from " << scriptsDir << " (absolute path: " << fs::absolute(scriptsDir) << ")"
         << endl;
    if (!fs::is_directory(scriptsDir)) {
        cout << scriptsDir << " is NOT a valid directory!" << endl;
        return;
    }

    for (const auto& script : fs::directory_iterator(scriptsDir)) {
        std::cout << "Loading script: " << script.path() << std::endl;

        std::ifstream t(script.path());
        std::stringstream buffer;
        buffer << t.rdbuf();

        cout << buffer.str() << endl;

        const bool status = db->Exec(buffer.str().c_str());

        cout << "---> status: " << status << endl;
    }
}

TSQLServer* DbHandler::connect() {
    const std::string uri = schema + "://" + host + ":" + port + "/" + database;
    cout << "Attempting to connect to: " << uri << endl;
    TSQLServer* server = TSQLServer::Connect(uri.c_str(), user.c_str(), password.c_str());
    return server;
}

DbHandler::~DbHandler() { delete db; }

void DbHandler::insertFiles(const set<FileInfo>& files) {
    const int chunkSize = 50000;

    const string startingSql =
        "INSERT INTO files "
        " (path, stat_uid, stat_gid, stat_atim, stat_mtim, stat_ctim, stat_size) "
        "VALUES ";

    string query = startingSql;

    int counter = 0;
    int inserted = 0;

    for (const auto& file : files) {
        const string value =
            "("
            "'" +
            file.absolute_path + "'" + ", " + "'" + file.stat_user + "'" + ", " + "'" + file.stat_group +
            "'" + ", " + "to_timestamp(" + to_string(file.stat_atime) + ")" + ", " + "to_timestamp(" +
            to_string(file.stat_mtime) + ")" + ", " + "to_timestamp(" + to_string(file.stat_ctime) + ")" +
            ", " + to_string(file.stat_size) + "),";

        query += value;

        counter++;

        if (counter >= chunkSize || counter + inserted == files.size()) {
            query.pop_back();
            query += ";";  // replace the last ',' for a ';'

            inserted += counter;

            cout << "Attempting to insert " << counter << " files into database. " << files.size() - inserted
                 << " remaining" << endl;
            // cout << query << endl;
            bool status = db->Exec(query.c_str());
            cout << "---> status: " << status << endl;

            query = startingSql;

            counter = 0;
        }
    }
}

int DbHandler::getNumberOfEntries() {
    const string query = "SELECT COUNT(path) FROM files";
    TSQLResult* result = db->Query(query.c_str());
    TSQLRow* next = result->Next();
    /*
    cout << result->GetFieldCount() << endl;
    cout << result->GetFieldName(int(0)) << endl;
    cout << next->GetField(0) << endl;
    */
    auto count = next->GetField(0);
    return stoi(string(count));
}

void DbHandler::InsertFullFile(const FileSchema& fileSchema) {
    /*
     * We need to:
     *  1. Update the main table setting "processed" to true
     *  2. Iterate over all the metadata classes and insert them into the corresponding tables, creating them
     * if necessary
     */

    const string path = fileSchema.fileInfo.absolute_path;
    for (const auto& [className, classMap] : fileSchema.fileMap) {
        if (!metadataTableNames.count(className)) {
            CreateAllMetadataTables(fileSchema);
        }

        for (const auto& [name, keysMap] : classMap) {
            string query = "";
            query += "INSERT INTO \"" + className + "\" ";
            query += " (";
            query += "path, name, ";
            for (const auto& [key, details] : keysMap) {
                query += "\"" + key + "\" ,";
            }

            query.pop_back();
            query += ")";
            query += "VALUES";
            query += "(";
            query += "'" + path + "','" + name + "',";
            for (const auto& [key, details] : keysMap) {
                std::string castValue = "CAST('" + details.value + "' AS " + sqlType(details.type) + ")"; // As not necessery
                query += castValue + ",";
            }
            query.pop_back();
            query += ");";

            // cout << query << endl;

            if (!db->Exec(query.c_str())) {
                cout << "---> WARNING: Error executing insert into '" << className << "': " << query << endl;
            }
        }
    }
    // set processed to true
    string query = "UPDATE files SET processed=true where path='" + path + "';";
    cout << query << endl;
    if (!db->Exec(query.c_str())) {
        cout << "---> WARNING: Error updating processed status to 'true' for '" << path << "': " << query
             << endl;
        throw std::runtime_error("Error updating status, this should always work");
    }
}

std::string DbHandler::sqlType(const std::string& type) {
    if (type == "string") {
        return "text";
    } else if (type == "int") {
        return "integer";
    } else if (type == "float") {
        return "real";
    } else {
        //throw std::runtime_error("Unknown type: " + type);
        return "text";
    }
}

void DbHandler::CreateMetadataTable(const std::string& className, std::vector<std::pair<std::string, std::string>>& keyTypes) {
    string query;
    query += "CREATE TABLE IF NOT EXISTS \"" + className + "\"";
    query += "(";

    query += "path text NOT NULL,";
    query += "name text NOT NULL,";

    for (const auto& [key, type] : keyTypes) {
        std::string sqlTypeReturn = sqlType(type);  
        query += "\"" + key + "\" " + sqlTypeReturn + ",";
    }

    query += "PRIMARY KEY(path, name),";
    query += "CONSTRAINT fk_path FOREIGN KEY(path) REFERENCES files(path) ON DELETE CASCADE";
    query += ");";

    // cout << query << endl;
    if (!db->Exec(query.c_str())) {
        cout << "---> WARNING: PROBLEM CREATING TABLE " << className << "! " << query << endl;
        throw std::runtime_error("Error creating table (if not exists). This should never return an error");
    }

    metadataTableNames.insert(className);
}

void DbHandler::CreateAllMetadataTables(const FileSchema& fileSchema) {
    for (const auto& [className, classMap] : fileSchema.fileMap) {
        vector<std::pair<string, string>> keyTypes;

        if (!classMap.empty()) {
            const auto& keysMap = classMap.begin()->second; // Assuming there's at least one entry

            for (const auto& [keyName, details] : keysMap) {
                keyTypes.emplace_back(keyName, details.type); // Store key and its type
            }

            CreateMetadataTable(className, keyTypes);
        }
    }
}
