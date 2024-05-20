//
// Created by Luis on 6/23/2021.
//

#include "FileSchema.h"

#include <sys/stat.h>

#include <iostream>

using namespace std;

void FileSchema::InsertDataMember(const string& className, const string& keyName,const string& dataMemberName,
                                     const string& dataMemberValue,const string& dataMemberType) {
    // Create the details struct and populate it
    DataMemberDetails details;
    details.value = dataMemberValue;
    details.type = dataMemberType;

    if (fileMap.count(className) == 0) {
        fileMap[className] = {};
    }

    if (fileMap[className].count(keyName) == 0) {
        fileMap[className][keyName] = {};
    }
    // HERE I NEED TO ADD THE DATA MEMBER TYPE
    fileMap[className][keyName][dataMemberName] = details;
}

void FileSchema::Print() {
    cout << endl;
    for (const auto& [className, classMap] : fileMap) {
        cout << "-> " << className << ":" << endl;
        for (const auto& [keyName, metadataMap] : classMap) {
            cout << "---> " << keyName << endl;
            for (const auto& [dataMemberName, dataMemberValue] : metadataMap) {
                // cout << "------> " << dataMemberName << ": " << dataMemberValue << endl;
            }
        }
    }
    cout << endl;
}

bool checkForInvalidCharacters(const string& s) {
    for (const auto& c : string(s)) {
        if (static_cast<unsigned char>(c) > 127) {
            return true;
        }
    }
    return false;
}

FileInfo FileSchema::getFileInfoFromPath(std::filesystem::path filepath) {
    FileInfo file{};

    const string extension = ".root";
    struct stat stats {};

    // check if file has .root extension
    if (filepath.extension() != extension) {
        return file;
    }

    if (stat(filepath.c_str(), &stats) == 0) {
        // file info read correctly
        if (checkForInvalidCharacters(string(filepath))) {
            return file;
        }

        file.absolute_path = filepath;

        file.stat_atime = stats.st_atime;
        file.stat_mtime = stats.st_mtime;
        file.stat_ctime = stats.st_ctime;

        file.stat_user = to_string(stats.st_uid);
        file.stat_group = to_string(stats.st_gid);

        file.stat_size = stats.st_size;
    }

    return file;
}
