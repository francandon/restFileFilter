
#ifndef RESTFILESYSTEMINDEXER_NOTIFY_H
#define RESTFILESYSTEMINDEXER_NOTIFY_H

// https://www.opensourceforu.com/2011/04/getting-started-with-inotify/

#include <sys/inotify.h>

#include <map>
#include <set>

#include "FileSchema.h"

#define MAX_LEN 1024    /*Path length for a directory*/
#define MAX_EVENTS 1024 /*Max. number of events to process at one go*/
#define LEN_NAME 16     /*Assuming that the length of the filename won't exceed 16 bytes*/
#define EVENT_SIZE (sizeof(struct inotify_event))      /*size of one event*/
#define BUF_LEN (MAX_EVENTS * (EVENT_SIZE + LEN_NAME)) /*buffer to store the data of events*/

class Notify {
   public:
    Notify(std::set<FileInfo>&);

    void Watch();

    inline void setContainer(std::set<FileInfo>& newContainer) { container = newContainer; }

   private:
    static Notify* pinstance_;
    std::set<FileInfo>& container;
    std::map<int, std::string> wdToPath;
};

#endif  // RESTFILESYSTEMINDEXER_NOTIFY_H
