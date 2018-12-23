//
// Created by Zhengxiao Du on 2018-12-20.
//

#include "Table.h"
#include "../rm/RecordManager.h"
#include "sm.h"


int Table::getOffset(std::string attribute) const {
    for (const auto &it: attrInfos) {
        if (attribute == it.attrName) {
            return it.attrOffset;
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

Table::Table(const std::string &tableName) {
    this->tableName = tableName;
    int rc = SM_Manager::getInstance().GetTableInfo(tableName.c_str(), columns, tableConstraints);
    if (rc != 0) {
        throw "The record of relation table " + tableName + " does not exist\n";
    }
    int offset = 0;
    for (const auto &it: columns.columns) {
        AttrInfo attrInfo;
        attrInfo.tableName = tableName;
        attrInfo.attrName = it->columnName;
        attrInfo.attrType = it->type;
        attrInfo.attrLength = it->size;
        attrInfo.notNull = static_cast<bool>(it->columnFlag & COLUMN_FLAG_NOTNULL);
        attrInfo.withIndex = true;
        attrInfo.attrOffset = offset;
        offset += it->size;
        indexHandles.push_back(nullptr);
    }
    rc = tryOpenFile();
    if (rc != 0) {
        throw "The file of relation table " + tableName + " does not exist\n";
    }
}

int Table::deleteData(const RID &rid) {
    int rc;
    RM_Record record;
    fileHandle.getRec(rid, record);
    for (int i = 0; i < attrInfos.size(); ++i) {
        if (attrInfos[i].withIndex) {
            rc = tryOpenIndex(i);
            if (rc != 0) {
                attrInfos[i].withIndex = false;
                continue;
            }
            indexHandles[i]->DeleteEntry(record.getData() + attrInfos[i].attrOffset, rid);
        }
    }
    fileHandle.deleteRec(rid);
    return 0;
}

Table::~Table() {
    for (auto &it: indexHandles) {
        delete it;
    }
}

int Table::tryOpenIndex(int indexNo) {
    if (indexHandles[indexNo] == nullptr) {
        auto *indexHandle = new IX_IndexHandle();
        int rc = IX_Manager::getInstance().OpenIndex(tableName.c_str(), indexNo, *indexHandle);
        if (rc != 0) {
            delete indexHandle;
            return -1;
        }
        indexHandles[indexNo] = indexHandle;
    }
    return 0;
}

