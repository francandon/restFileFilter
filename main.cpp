//
// Created by lobis on 6/18/2021.
//

// ROOT
#include <TClass.h>
#include <TFile.h>
#include <TKey.h>
#include <TROOT.h>
#include <TSystem.h>
#include <sys/stat.h>
#include <sys/types.h>

// REST
#include <TRestMetadata.h>
#include <TRestRun.h>

#include <chrono>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <functional>
#include <iomanip>
#include <iostream>
#include <map>
#include <queue>
#include <set>
#include <thread>
#include <vector>

#include "DbHandler.h"
#include "FileSchema.h"
#include "Notify.h"

namespace fs = std::filesystem;
using namespace std::chrono_literals;
using namespace std;
using namespace ROOT;

void loadRestLibs() {
    cout << "Loading REST libraries" << endl;
    TRestRun::Class();
    cout << "Finished loading REST libraries" << endl;
}

FileSchema processFile(const FileInfo& fileInfo) {
    const string filepath = fileInfo.absolute_path;

    cout << "Processing file: " << filepath << endl;

    const string kMETADATA = "TRestMetadata";

    auto file = TFile(filepath.c_str(), "READ");

    FileSchema fileMap;
    fileMap.setFileInfo(fileInfo);

    for (const auto& key : *file.GetListOfKeys()) {
        const auto keyName = key->GetName();
        const string className = ((TKey*)key)->GetClassName();  // cast to string

        if (!TClass::GetDict(className.c_str())) {
            // cout << "---> WARNING: NO DICT FOUND FOR '" << className << "' in " << file.GetName() << "
            // skipping..." << endl;
            continue;
        } else {
            // cout << "---> DEBUG: DICT FOUND FOR '" << className << "' in " << file.GetName() << endl;
        }

        const TObject* object = file.Get(keyName);
        if (!object->InheritsFrom(kMETADATA.c_str())) {
            continue;
        }

        TRestMetadata* data = (TRestMetadata*)object;

        auto list = data->IsA()->GetListOfDataMembers();

        for (const auto obj : *list) {
            TDataMember* dataMember = static_cast<TDataMember*>(obj);
            if (dataMember->IsPersistent()) {
                string dataMemberName = string(obj->GetName());
                if (dataMemberName == "fgIsA") continue;  // skip this one
                string dataMemberValue = data->GetDataMemberValue(string(obj->GetName()));
                // Getting the data member type
                TDataType* dataType = dataMember->GetDataType();
                string dataMemberType;
                if (dataType) {
                    dataMemberType = dataType->GetName(); // Get the type name
                } else {
                    dataMemberType = "complex or custom type"; // For non-primitive data types
                }

                // Inserting both value and type into fileMap
                fileMap.InsertDataMember(className, keyName, dataMemberName, dataMemberValue, dataMemberType);
            }
        }
    }
    return fileMap;
}

void indexFiles(const string& startingPath, std::set<FileInfo>& container) {
    auto startingSize = container.size();
    int count = 0;
    int countRootFiles = 0;

    const std::string extension = ".root";
    struct stat stats {};

    for (const auto& entry :
         fs::recursive_directory_iterator(startingPath, fs::directory_options::skip_permission_denied)) {
        count += 1;

        const auto& filepath = entry.path();

        FileInfo fileInfo = FileSchema::getFileInfoFromPath(filepath);

        if (fileInfo.absolute_path != "") {
            container.insert(fileInfo);
        }
    }

    cout << "'indexFiles' processed " << count << " files, of which " << countRootFiles
         << " were ROOT files, and inserted " << container.size() - startingSize
         << " new elements into the container, which now contains " << container.size() << " elements"
         << endl;
}

int main() {
    loadRestLibs();

    auto db = DbHandler::Instance();

    std::set<FileInfo> container;

    indexFiles("/", container);

    auto notify = new Notify(container);
    std::thread t([notify] { notify->Watch(); });

    db->insertFiles(container);

    int count = db->getNumberOfEntries();
    cout << "There are " << count << " elements in the database" << endl;

    int processedFilesCount = 0;

    while (!container.empty()) {
        if (container.empty()) {
            continue;
        }
        auto fileInfo = *container.begin();
        container.erase(container.begin());

        processedFilesCount += 1;
        cout << processedFilesCount << " of " << container.size() << " timestamp: " << fileInfo.stat_mtime
             << endl;

        if (fileInfo.stat_mtime < 1621521836) {
            continue;
        }

        cout << "Before processFile" << endl;
        const FileSchema file = processFile(fileInfo);
        cout << "After processFile, before InsertFullFile" << endl;
        db->InsertFullFile(file);
        cout << "After InsertFullFile" << endl;
    }

    cout << "DONE!" << endl;
}
