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
    RM_FileHandle();                                  // Constructor
    ~RM_FileHandle();                                  // Destructor
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
};

#endif //MYDB_RM_FILEHANDLE_H
