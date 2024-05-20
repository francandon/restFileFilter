//
// Created by Luis on 6/22/2021.
//

#include "Notify.h"

#include <sys/inotify.h>
#include <unistd.h>

#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <stdexcept>

namespace fs = std::filesystem;
using namespace std;

Notify::Notify(std::set<FileInfo>& newContainer) : container(newContainer) {}

void Notify::Watch() {
    int fd;
    fd = inotify_init();
    if (fd < 0) {
        throw std::runtime_error("Failed to initialize 'inotify'");
    }

    int length, i = 0, wd;

    for (auto& p : fs::recursive_directory_iterator("/storage")) {
        if (fs::is_symlink(p) || !fs::is_directory(p)) continue;

        wd = inotify_add_watch(fd, p.path().c_str(), IN_CREATE | IN_MODIFY | IN_DELETE);
        if (wd < 0) {
            throw std::runtime_error("Failed to add watch");
        } else {
            string s = p.path();
            wdToPath[wd] = s;
        }
    }

    char buffer[BUF_LEN];

    while (true) {
        i = 0;
        length = read(fd, buffer, BUF_LEN);

        if (length < 0) {
            cout << "ERROR!" << endl;
        }

        while (i < length) {
            auto event = (struct inotify_event*)&buffer[i];
            i += EVENT_SIZE + event->len;

            if (event->mask & IN_ISDIR) {
                // only want to watch for file changes
                continue;
            }

            auto filepath = fs::path(wdToPath[event->wd] + "/" + event->name);
            FileInfo fileInfo = FileSchema::getFileInfoFromPath(filepath);
            if (fileInfo.absolute_path == "") {
                continue;
            }

            if (event->len) {
                if (event->mask & IN_CREATE) {
                    cout << "Created: " << fileInfo.absolute_path << endl;
                    container.insert(fileInfo);
                }

                if (event->mask & IN_MODIFY) {
                    // cout << "Modified: " << fileInfo.absolute_path << endl;
                    // container.insert(fileInfo);
                }
            }

            if (event->mask & IN_DELETE) {
                // TODO FIX DELETE; NOT WORKING
                cout << "Deleted: " << fileInfo.absolute_path << endl;
                container.erase(fileInfo);
            }

            // cout << "CONTAINER: " << container.size() << endl;
        }
    }
}
