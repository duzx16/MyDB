//
// Created by Zhengxiao Du on 2018-12-20.
//

#ifndef MYDB_TABLE_H
#define MYDB_TABLE_H

#include "../parser/Tree.h"
#include "../rm/RM_FileHandle.h"
#include "../rm/RecordManager.h"

class Table {
public:
    int getOffset(std::string attribute) const;
    ColumnNode * getColumn(std::string attribute) const;
    std::string checkData(char *data) const;

    RM_FileHandle &getFileHandler();

    int tryOpenFile();

    std::string tableName;
    ColumnDecsList columns;
    TableConstraintList tableConstraints;
    RM_FileHandle fileHandle;
};


#endif //MYDB_TABLE_H
