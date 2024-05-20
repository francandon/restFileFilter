//
// Created by Luis on 6/23/2021.
//

#ifndef RESTFILESYSTEMINDEXER_FILESCHEMA_H
#define RESTFILESYSTEMINDEXER_FILESCHEMA_H

#include <filesystem>
#include <map>
#include <string>

struct FileInfo {
    std::string absolute_path;  // must be unique
    std::string stat_user;
    std::string stat_group;
    uint stat_atime;
    uint stat_mtime;
    uint stat_ctime;
    uint stat_size;

    bool operator<(const FileInfo& a) const {
        if (stat_mtime == a.stat_mtime) {
            return (absolute_path > a.absolute_path);
        }
        return (stat_mtime > a.stat_mtime);
    }

    bool operator==(const FileInfo& a) const { return (absolute_path == a.absolute_path); }
};

class FileSchema {
   public:

   struct DataMemberDetails {
        std::string value;
        std::string type;
    };
    FileInfo fileInfo;
    std::map<std::string, std::map<std::string, std::map<std::string, DataMemberDetails>>> fileMap;

   public:
    void Print();
    void InsertDataMember(const std::string& className, const std::string& keyName,
                          const std::string& dataMemberName, const std::string& dataMemberValue, const std::string& dataMemberType);
    inline void setFileInfo(const FileInfo& newFileInfo) { fileInfo = newFileInfo; }
    static FileInfo getFileInfoFromPath(std::filesystem::path filepath);
};

#endif  // RESTFILESYSTEMINDEXER_FILESCHEMA_H
