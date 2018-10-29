//
// Created by Zhengxiao Du on 2018/10/28.
//

#ifndef MYDB_RM_FILESCAN_H
#define MYDB_RM_FILESCAN_H

#include "RM_FileHandle.h"
#include "../utils/MyBitMap.h"

class RM_FileScan {
private:
    const RM_FileHandle *_file_handle = nullptr;
    AttrType _attr_type = AttrType::NO_ATTR;
    int _attr_length;
    int _attr_offset;
    CompOp _comp_op = CompOp::NO_OP;
    void *_value = nullptr;
    unsigned _current_page;
    MyBitMap _current_bitmap = MyBitMap(0, 1);
    char *_current_bitdata;
public:
    RM_FileScan() = default;                                // Constructor
    ~RM_FileScan() = default;                                // Destructor
    int openScan(const RM_FileHandle &fileHandle,  // Initialize file scan
                 AttrType attrType,
                 int attrLength,
                 int attrOffset,
                 CompOp compOp,
                 void *value
    );

    int getNextRec(RM_Record &rec);                  // Get next matching record
    int closeScan();                                // Terminate file scan
};

#endif //MYDB_RM_FILESCAN_H
