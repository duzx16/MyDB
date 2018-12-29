//
// Created by Zhengxiao Du on 2018-12-20.
//

#include "Table.h"
#include "../rm/RecordManager.h"
#include "../sm/sm.h"
#include "../parser/Expr.h"
#include <memory>
#include <string.h>
#include <sstream>


int Table::getOffset(const std::string &attribute) const {
    for (const auto &it: attrInfos) {
        if (attribute == it.attrName) {
            return it.attrOffset;
        }
    }
    return -1;
}

ColumnNode *Table::getColumn(const std::string &attribute) const {
    for (const auto &it: columns.columns) {
        if (attribute == it->columnName)
            return it;
    }
    return nullptr;
}

int Table::getColumnIndex(const std::string &attribute) const {
    for (int i = 0; i < attrInfos.size(); ++i) {
        if (attribute == attrInfos[i].attrName) {
            return i;
        }
    }
    return -1;
}

const BindAttribute *Table::getAttrInfo(const std::string &attribute) const {
    int index = getColumnIndex(attribute);
    if (index >= 0) {
        return &(attrInfos[index]);
    } else {
        return nullptr;
    }
}

bool checkValueIn(void *left, const ConstValueList &constValues, AttrType type, int length) {
    for (const auto &it: constValues.constValues) {
        switch (type) {
            // todo assume the type is identical here
            case AttrType::INT:
            case AttrType::DATE:
                if (*reinterpret_cast<int *>(left) == it->value.i) {
                    return true;
                }
                break;
            case AttrType::FLOAT:
                if (*reinterpret_cast<float *>(left) == it->value.f) {
                    return true;
                }
                break;
            case AttrType::STRING:
            case AttrType::VARCHAR:
                if (strncmp(reinterpret_cast<char *>(left), it->value_s.c_str(), length) == 0) {
                    return true;
                }
                break;
            case AttrType::BOOL:
            case AttrType::NO_ATTR:
                break;
        }
    }
    return false;
}

int attributeAssign(void *data, const Expr &value, AttrType type, int length) {
    switch (type) {
        case AttrType::DATE:
            if (value.dataType != type) {
                return -1;
            }
            *reinterpret_cast<int *>(data) = value.value.i;
        case AttrType::INT:
            if (value.dataType == AttrType::INT) {
                *reinterpret_cast<int *>(data) = value.value.i;
            } else if (value.dataType == AttrType::FLOAT) {
                *reinterpret_cast<int *>(data) = static_cast<int>(value.value.f);
            } else {
                return -1;
            }
            break;
        case AttrType::FLOAT:
            if (value.dataType == AttrType::FLOAT) {
                *reinterpret_cast<float *>(data) = value.value.f;
            } else if (value.dataType == AttrType::INT) {
                *reinterpret_cast<float *>(data) = value.value.i;
            } else {
                return -1;
            }
            break;
        case AttrType::STRING:
        case AttrType::VARCHAR:
            if (value.dataType == AttrType::STRING || value.dataType == AttrType::VARCHAR) {
                strncpy(static_cast<char *>(data), value.value_s.c_str(), length);
            } else {
                return -1;
            }
            break;
        case AttrType::BOOL:
        case AttrType::NO_ATTR:
            return -1;
    }
    return 0;
}

std::string Table::checkData(char *data) {
    int rc;
    for (const auto &it: tableConstraints.tbDecs) {
        switch (it->type) {
            case ConstraintType::PRIMARY_CONSTRAINT: {
                int indexNo;
                for (indexNo = 0; indexNo < attrInfos.size(); indexNo++) {
                    // todo complete multiple primary keys
                    if (attrInfos[indexNo].attrName == it->column_list->idents[0]) {
                        break;
                    }
                }
                if (indexNo < attrInfos.size()) {
                    if (data[attrInfos[indexNo].attrOffset - 1] == 0) {
                        return "The primary key " + attrInfos[indexNo].attrName + " can't be null\n";
                    }
                    void *value = data + attrInfos[indexNo].attrOffset;
                    rc = tryOpenIndex(indexNo);
                    if (rc != 0) {
                        return "We are doomed! Can't find the index for the primary key\n";
                    }
                    IX_IndexScan indexScan;
                    indexScan.OpenScan(*indexHandles[indexNo], CompOp::EQ_OP, value);
                    RID rid;
                    rc = indexScan.GetNextEntry(rid);
                    indexScan.CloseScan();
                    if (rc == 0) {
                        return "The primary key " + attrInfos[indexNo].attrName + " alrealy exists\n";
                    }
                } else {
                    return "We are doomed! Can't find the primary key\n";
                }
            }
                break;
            case ConstraintType::FOREIGN_CONSTRAINT:
                // todo implement foreign key constraint here
                break;
            case ConstraintType::CHECK_CONSTRAINT: {
                int indexNo;
                for (indexNo = 0; indexNo < attrInfos.size(); indexNo++) {
                    if (attrInfos[indexNo].attrName == it->column_name) {
                        break;
                    }
                }
                if (indexNo < attrInfos.size()) {
                    const auto &info = attrInfos[indexNo];
                    if (data[info.attrOffset - 1] == 0) {
                        return "The value for " + info.attrName + " can't be null\n";
                    }
                    if (not checkValueIn(data + info.attrOffset, *it->const_values, info.attrType,
                                         info.attrLength)) {
                        return "The value for " + info.attrName + " is invalid\n";
                    }
                } else {
                    return "We are doomed! Can't find the constraint key\n";
                }
            }
                break;
        }
    }
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
    this->recordSize = 0;
    int rc = SM_Manager::getInstance()->GetTableInfo(tableName.c_str(), columns, tableConstraints);
    if (rc != 0) {
        throw "The record of relation table " + tableName + " does not exist\n";
    }
    int offset = 0;
    for (const auto &it: columns.columns) {
        BindAttribute attrInfo;
        attrInfo.tableName = tableName;
        attrInfo.attrName = it->columnName;
        attrInfo.attrType = it->type;
        attrInfo.attrLength = it->size;
        attrInfo.notNull = static_cast<bool>(it->columnFlag & COLUMN_FLAG_NOTNULL);
        attrInfo.withIndex = true;
        attrInfo.attrOffset = offset + 1;
        offset += it->size + 1;
        this->recordSize += it->size + 1;
        indexHandles.push_back(nullptr);
        attrInfos.push_back(std::move(attrInfo));
    }
    rc = tryOpenFile();
    if (rc != 0) {
        throw "The file of relation table " + tableName + " does not exist\n";
    }
}

int Table::deleteData(const RID &rid) {
    int rc;
    RM_Record record;
    rc = fileHandle.getRec(rid, record);
    if (rc != 0) {
        return -1;
    }
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
    rc = fileHandle.deleteRec(rid);
    if (rc != 0) {
        return -1;
    }
    return 0;
}

Table::~Table() {
    for (auto &it: indexHandles) {
        if (it != nullptr) {
            it->CloseIndex();
        }
        delete it;
    }
}

int Table::tryOpenIndex(int indexNo) {
    if (indexHandles[indexNo] == nullptr) {
        if (!IX_Manager::indexAvailable()) {
            return -1;
        }
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

int Table::insertData(const IdentList *columnList, const ConstValueList *constValues) {
    int rc;
    if (columnList == nullptr) {
        if (constValues->constValues.size() != attrInfos.size()) {
            throw std::string("The number of inserted values doesn't match that of columns\n");
        }
        std::unique_ptr<char[]> data{new char[recordSize]};
        for (int i = 0; i < attrInfos.size(); ++i) {
            const auto &info = attrInfos[i];
            const Expr &value = *(constValues->constValues[i]);
            data[info.attrOffset - 1] = 1;
            rc = attributeAssign(data.get() + info.attrOffset, value, info.attrType, info.attrLength);
            if (rc != 0) {
                throw std::string("The type of inserted value doesn't match that of column " + info.attrName + "\n");
            }
        }
        auto result = checkData(data.get());
        if (!result.empty()) {
            throw std::string{result};
        }
        // todo check insert success here
        RID rid = fileHandle.insertRec(data.get());
        try {
            insertIndex(data.get(), rid);
        }
        catch (const std::string &strerror) {
            fileHandle.deleteRec(rid);
            throw;
        }
    } else {
        // todo implement optional columns
    }
    return 0;
}

int Table::insertIndex(char *data, const RID &rid) {
    int rc;
    for (int i = 0; i < attrInfos.size(); ++i) {
        rc = tryOpenIndex(i);
        if (rc == 0) {
            rc = indexHandles[i]->InsertEntry(data + attrInfos[i].attrOffset, rid);
            if (rc != 0) {
                for (int indexNo = 0; indexNo < i; ++indexNo) {
                    rc = tryOpenIndex(indexNo);
                    if (rc == 0) {
                        indexHandles[indexNo]->DeleteEntry(data + attrInfos[indexNo].attrOffset, rid);
                    }
                }
                throw "Index insert failed for column " + attrInfos[i].attrName + "\n";
            }
        }
    }
    return 0;
}

int Table::updateData(const RM_Record &record, const std::vector<int> &attrIndexes, SetClauseList *setClauses) {
    int rc;
    std::string result;
    char *data = record.getData();
    for (int i = 0; i < attrIndexes.size(); ++i) {
        int indexNo = attrIndexes[i];
        const auto &info = attrInfos[indexNo];
        // the type should have been checked
        rc = attributeAssign(data + info.attrOffset, *setClauses->clauses[i].second, info.attrType, info.attrLength);
    }
    // todo update index
    result = checkData(data);
    if (!result.empty()) {
        throw std::string(result);
    }
    fileHandle.updateRec(record);
    return 0;
}

std::string Table::printData(const char *data) {
    std::ostringstream stream;
    for (const auto &attr: attrInfos) {
        switch (attr.attrType) {
            case AttrType::INT:
                stream << *reinterpret_cast<const int *>(data + attr.attrOffset);
                break;
            case AttrType::FLOAT:
                stream << *reinterpret_cast<const float *>(data + attr.attrOffset);
                break;
            case AttrType::STRING:
            case AttrType::VARCHAR:
                stream << (data + attr.attrOffset);
                break;
            case AttrType::DATE:
                //todo implement this
                break;
            case AttrType::BOOL:
                break;
            case AttrType::NO_ATTR:
                break;
        }
        stream << "\t";
    }
    return stream.str();
}

