
#ifndef RESTFILESYSTEMINDEXER_DBHANDLER_H
#define RESTFILESYSTEMINDEXER_DBHANDLER_H

#include <TSQLResult.h>
#include <TSQLRow.h>
#include <TSQLServer.h>

#include <filesystem>
#include <set>
#include <string>
#include <vector>
#include <utility> // For std::pair

#include "FileSchema.h"

class DbHandler {
   private:
    DbHandler();
    static DbHandler* pinstance_;

    const std::string schema;
    const std::string host;
    const std::string port;
    const std::string database;
    const std::string user;
    const std::string password;

    TSQLServer* db;
    TSQLServer* connect();

    const std::filesystem::path scriptsDir;

    std::set<std::string> metadataTableNames;

   public:
    static DbHandler* Instance();
    ~DbHandler();

    void insertFiles(const std::set<FileInfo>&);
    void InsertFullFile(const FileSchema& fileSchema);
    void CreateMetadataTable(const std::string& className, std::vector<std::pair<std::string, std::string>>& keyTypes);
    void CreateAllMetadataTables(const FileSchema& fileSchema);
    std::string sqlType(const std::string& type);

    int getNumberOfEntries();
};

#endif  // RESTFILESYSTEMINDEXER_DBHANDLER_H
