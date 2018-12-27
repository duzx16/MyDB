//
// Created by Zhengxiao Du on 2018/10/28.
//

#ifndef MYDB_RECORDMANAGER_H
#define MYDB_RECORDMANAGER_H

#include "RM_FileHandle.h"
#include "RM_FileScan.h"
#include "HeaderPage.h"
#include "../pf/pf.h"

#include <string>

class RecordManager {
public:
    ~RecordManager() = default;

    int createFile (std::string, unsigned record_size);
    int destroyFile(std::string filename);
    int openFile(std::string filename, RM_FileHandle &file_handle);
    int closeFile(RM_FileHandle &file_handle);
    static RecordManager & getInstance();
private:
	RecordManager();
    PF_Manager & _pf_manager;
};

#endif //MYDB_RECORDMANAGER_H
