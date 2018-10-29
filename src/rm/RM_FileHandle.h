//
// Created by Zhengxiao Du on 2018/10/28.
//

#ifndef MYDB_RM_FILEHANDLE_H
#define MYDB_RM_FILEHANDLE_H

#include "HeaderPage.h"
#include "RM_Record.h"

#include "RID.h"
#include "../Constants.h"

class RM_FileHandle {
public:
    friend class RecordManager;

    RM_FileHandle() = default;                                  // Constructor
    ~RM_FileHandle();                                  // Destructor

    RM_Record getRec(const RID &rid) const;

    // Get a record
    RID insertRec(const char *pData);       // Insert a new record,
    //   return record id
    int deleteRec(const RID &rid);                    // Delete a record
    int updateRec(const RM_Record &rec);              // Update a record
    int forcePages(unsigned pageNum) const; // Write dirty page(s) to disk
private:
    int insertPage();

    unsigned getOffset(unsigned slot_num) const;

    HeaderPage _header_page;
    bool _header_modified;
    int _fileID;
    bool _initialized = false;
};

#endif //MYDB_RM_FILEHANDLE_H
