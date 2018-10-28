//
// Created by Zhengxiao Du on 2018/10/28.
//

#ifndef MYDB_RECORDMANAGER_H
#define MYDB_RECORDMANAGER_H

#include "../fileio/BufPageManager.h"
#include "../fileio/FileManager.h"
#include "RM_FileHandle.h"
#include "RM_FileScan.h"
#include "HeaderPage.h"

#include <string>

class RecordManager {
public:
    RecordManager(FileManager &);
    ~RecordManager();

    int createFile (std::string, unsigned record_size);
    int destroyFile(std::string filename);
    int openFile(std::string filename, RM_FileHandle &file_handle);
    int closeFile(RM_FileHandle &file_handle);

private:
    FileManager &_file_manager;
    BufPageManager _page_manager;
};

#endif //MYDB_RECORDMANAGER_H
