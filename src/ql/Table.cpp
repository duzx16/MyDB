//
// Created by Zhengxiao Du on 2018-12-20.
//

#include "Table.h"
#include "../rm/RecordManager.h"


int Table::getOffset(std::string attribute) const {
    int offset = 0;
    for (const auto &it: columns.columns) {
        if (attribute == it->columnName) {
            return offset;
        } else {
            offset += it->size;
        }
    }
    return -1;
}

ColumnNode *Table::getColumn(std::string attribute) const {
    for (const auto &it: columns.columns) {
        if (attribute == it->columnName)
            return it;
    }
    return nullptr;
}

std::string Table::checkData(char *data) const {
    return std::string();
}

RM_FileHandle &Table::getFileHandler() {
    return fileHandle;
}

int Table::tryOpenFile() {
    if (not fileHandle.is_initialized()) {
        return RecordManager::getInstance().openFile(tableName, fileHandle);
    }
    return 0;
}
