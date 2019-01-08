//
// Created by Zhengxiao Du on 2018/10/28.
//

#ifndef MYDB_RM_FILESCAN_H
#define MYDB_RM_FILESCAN_H

#include "RM_FileHandle.h"
#include "../utils/MyBitMap.h"
#include "../parser/Expr.h"
#include <string>

class RM_FileScan {
private:
    const RM_FileHandle *_file_handle = nullptr;
    Expr *_condition;
    unsigned _current_page;
    MyBitMap _current_bitmap = MyBitMap(0, 1);
    char *_current_bitdata = nullptr;
    std::string tableName;
public:
    RM_FileScan() = default;                                // Constructor
    ~RM_FileScan();                                // Destructor
    int openScan(const RM_FileHandle &fileHandle,  // Initialize file scan
                 Expr *condition, const std::string &tableName
    );

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
