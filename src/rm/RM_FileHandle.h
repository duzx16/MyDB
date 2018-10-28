//
// Created by Zhengxiao Du on 2018/10/28.
//

#ifndef MYDB_RM_FILEHANDLE_H
#define MYDB_RM_FILEHANDLE_H

#include "HeaderPage.h"
#include "RM_Record.h"

#include "RID.h"

class RM_FileHandle {
public:
    friend class RecordManager;

    RM_FileHandle() = default;                                  // Constructor
    ~RM_FileHandle() = default;                                  // Destructor

    void setRecordSize(unsigned size)
    {
        _header_page.recordSize = size;
        _header_modified = true;
    }

    int getRec(const RID &rid, RM_Record &rec) const;

    // Get a record
    int insertRec(const char *pData, RID &rid);       // Insert a new record,
    //   return record id
    int deleteRec(const RID &rid);                    // Delete a record
    int updateRec(const RM_Record &rec);              // Update a record
    int forcePages(unsigned pageNum) const; // Write dirty page(s) to disk
private:
    HeaderPage _header_page;
    bool _header_modified;
    int _fileID;
};

#endif //MYDB_RM_FILEHANDLE_H
