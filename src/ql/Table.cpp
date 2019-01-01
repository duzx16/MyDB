//
// Created by Zhengxiao Du on 2018-12-20.
//

#include "Table.h"
#include "../rm/RecordManager.h"
#include "../sm/sm.h"
#include "../parser/Expr.h"
#include "../utils/Date.h"
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

//ColumnNode *Table::getColumn(const std::string &attribute) const {
//    for (const auto &it: columns.columns) {
//        if (attribute == it->columnName)
//            return it;
//    }
//    return nullptr;
//}

int Table::getColumnIndex(const std::string &attribute) const {
    for (int i = 0; i < attrInfos.size(); ++i) {
        if (attribute == attrInfos[i].attrName) {
            return i;
        }
    }
    return -1;
}

const BindAttribute &Table::getAttrInfo(int index) const {
    return attrInfos[index];
}

int Table::getAttrCount() const {
    return attrInfos.size();
}

bool Table::getIndexAvailable(int index) {
    return tryOpenIndex(index);
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
            if (value.dataType == AttrType::DATE) {
                *reinterpret_cast<int *>(data) = value.value.i;
            } else if (value.dataType == AttrType::VARCHAR or value.dataType == AttrType::STRING) {
                int date = parseData(value.value_s.c_str());
                if (date >= 0) {
                    *reinterpret_cast<int *>(data) = date;
                } else {
                    return -1;
                }
            } else {
                return -1;
            }
            break;
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
    int rc, i = 0;
    for (const auto &it: tableConstraints.tbDecs) {
        int indexNo = constrAttrI[i];
        void *value = data + attrInfos[indexNo].attrOffset;
        const auto &info = attrInfos[indexNo];
        switch (it->type) {
            case ConstraintType::PRIMARY_CONSTRAINT: {
                if (data[info.attrOffset - 1] == 0) {
                    return "The primary key " + info.attrName + " can't be null\n";
                }
                rc = tryOpenIndex(indexNo);
                if (rc != 0) {
                    if (IX_Manager::indexAvailable())
                        return "We are doomed! Can't find the index for the primary key\n";
                    else
                        continue;
                }
                IX_IndexScan indexScan;
                indexScan.OpenScan(*indexHandles[indexNo], CompOp::EQ_OP, value);
                RID rid;
                rc = indexScan.GetNextEntry(rid);
                indexScan.CloseScan();
                if (rc == 0) {
                    return "The primary key " + info.attrName + " alrealy exists\n";
                }

            }
                break;
            case ConstraintType::FOREIGN_CONSTRAINT: {
                rc = tryOpenForeignIndex(i);
                if (rc != 0) {
                    if (IX_Manager::indexAvailable())
                        return "Can't find the index for the foreign key\n";
                    else
                        continue;
                }
                IX_IndexScan indexScan;
                indexScan.OpenScan(*foreignIndexs[i], CompOp::EQ_OP, value);
                RID rid;
                rc = indexScan.GetNextEntry(rid);
                indexScan.CloseScan();
                if (rc != 0) {
                    return "The foreign key for " + info.attrName + " doesn't exist\n";
                }
            }
                break;
            case ConstraintType::CHECK_CONSTRAINT: {
                if (data[info.attrOffset - 1] == 0) {
                    return "The value for " + info.attrName + " can't be null\n";
                }
                if (not checkValueIn(data + info.attrOffset, *it->const_values, info.attrType,
                                     info.attrSize)) {
                    return "The value for " + info.attrName + " is invalid\n";
                }
            }
                break;
        }
        i += 1;
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
    rc = tryOpenFile();
    if (rc != 0) {
        throw "The file of relation table " + tableName + " does not exist\n";
    }
    int offset = 0;
    for (const auto &it: columns.columns) {
        BindAttribute attrInfo;
        attrInfo.tableName = tableName;
        attrInfo.attrName = it->columnName;
        attrInfo.attrType = it->type;
        if (it->type == AttrType::STRING) {
            attrInfo.attrLength = it->size - 1;
        } else {
            attrInfo.attrLength = it->size;
        }
        if (it->type == AttrType::INT) {
            attrInfo.attrSize = sizeof(int);
        } else {
            attrInfo.attrSize = it->size;
        }
        attrInfo.notNull = static_cast<bool>(it->columnFlag & COLUMN_FLAG_NOTNULL);
        attrInfo.withIndex = true;
        attrInfo.attrOffset = offset + 1;
        offset += attrInfo.attrSize + 1;
        this->recordSize += attrInfo.attrSize + 1;
        indexHandles.push_back(nullptr);
        attrInfos.push_back(std::move(attrInfo));
    }
    for (const auto &it: tableConstraints.tbDecs) {
        for (int i = 0; i < attrInfos.size(); ++i) {
            if (attrInfos[i].attrName == it->column_name) {
                constrAttrI.push_back(i);
                break;
            }
        }
        foreignIndexs.push_back(nullptr);
        switch (it->type) {
            case ConstraintType::CHECK_CONSTRAINT:
            case ConstraintType::PRIMARY_CONSTRAINT:
                foreignAttrInt.push_back(-1);
                break;
            case ConstraintType::FOREIGN_CONSTRAINT: {
                ColumnDecsList foreign_columns;
                TableConstraintList foreign_constraint;
                rc = SM_Manager::getInstance()->GetTableInfo(it->foreign_table.c_str(), foreign_columns,
                                                             foreign_constraint);
                if (rc != 0) {
                    throw "The record of relation table " + it->foreign_table + " does not exist\n";
                }
                bool found = false;
                for (int i = 0; i < foreign_columns.columns.size(); ++i) {
                    if (foreign_columns.columns[i]->columnName == it->foreign_column) {
                        foreignAttrInt.push_back(i);
                        found = true;
                        break;
                    }
                }
                if (not found) {
                    throw "Can't find column " + it->foreign_column + " in table " + it->foreign_table + "\n";
                }
                break;
            }

        }
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

int Table::tryOpenForeignIndex(int constNo) {
    if (foreignIndexs[constNo] == nullptr) {
        if (!IX_Manager::indexAvailable()) {
            return -1;
        }
        auto *indexHandle = new IX_IndexHandle();
        int rc = IX_Manager::getInstance().OpenIndex(tableConstraints.tbDecs[constNo]->foreign_table.c_str(),
                                                     foreignAttrInt[constNo], *indexHandle);
        if (rc != 0) {
            delete indexHandle;
            return -1;
        }
        foreignIndexs[constNo] = indexHandle;
    }
    return 0;
}

int Table::insertData(const IdentList *columnList, const ConstValueList *constValues) {
    int rc;
    if (columnList == nullptr) {
        if (constValues->constValues.size() != attrInfos.size()) {
            cerr << std::string("The number of inserted values doesn't match that of columns\n");
            return -1;
        }
    } else {
        if (columnList->idents.size() != constValues->constValues.size()) {
            cerr << std::string("The number of inserted values doesn't match that of columns\n");
            return -1;
        }
    }
    std::unique_ptr<char[]> data{new char[recordSize]};
    for (int i = 0; i < attrInfos.size(); ++i) {
        const auto &info = attrInfos[i];
        int valueIndex;
        bool isNull;
        if (columnList == nullptr) {
            valueIndex = i;
            isNull = constValues->constValues[i]->is_null;
        } else {
            for (valueIndex = 0; valueIndex < columnList->idents.size(); ++valueIndex) {
                if (info.attrName == columnList->idents[valueIndex]) {
                    break;
                }
            }
            if (valueIndex < columnList->idents.size()) {
                isNull = constValues->constValues[valueIndex]->is_null;
            } else {
                isNull = true;
            }
        }
        if (isNull) {
            if (info.notNull) {
                cerr << "The column " << info.attrName << " can't be null\n";
                return -1;
            } else {
                data.get()[info.attrOffset - 1] = 0;
            }
        } else {
            const Expr &value = *(constValues->constValues[valueIndex]);
            data[info.attrOffset - 1] = 1;
            rc = attributeAssign(data.get() + info.attrOffset, value, info.attrType, info.attrSize);
            if (rc != 0) {
                cerr << std::string("The type of inserted value doesn't match that of column " + info.attrName + "\n");
                return -1;
            }
        }
    }
    auto result = checkData(data.get());
    if (!result.empty()) {
        cerr << std::string{result};
        return -1;
    }
    // todo check insert success here
    RID rid = fileHandle.insertRec(data.get());
    rc = insertIndex(data.get(), rid);
    if (rc != 0) {
        fileHandle.deleteRec(rid);
        return -1;
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
                cerr << "Index insert failed for column " + attrInfos[i].attrName + "\n";
                return rc;
            }
        }
    }
    return 0;
}

int Table::deleteIndex(char *data, const RID &rid) {
    int rc;
    for (int i = 0; i < attrInfos.size(); ++i) {
        rc = tryOpenIndex(i);
        if (rc == 0) {
            rc = indexHandles[i]->DeleteEntry(data + attrInfos[i].attrOffset, rid);
            if (rc != 0) {
                for (int indexNo = 0; indexNo < i; ++indexNo) {
                    rc = tryOpenIndex(indexNo);
                    if (rc == 0) {
                        indexHandles[indexNo]->InsertEntry(data + attrInfos[indexNo].attrOffset, rid);
                    }
                }
                cerr << "Index delete failed for column " + attrInfos[i].attrName + "\n";
                return rc;
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
        if (setClauses->clauses[i].second->is_null) {
            if (info.notNull) {
                cerr << "The column " << info.attrName << " can't be null\n";
                return -1;
            } else {
                data[info.attrOffset - 1] = 0;
            }

        } else {
            data[info.attrOffset - 1] = 1;
            rc = attributeAssign(data + info.attrOffset, *setClauses->clauses[i].second, info.attrType, info.attrSize);
            if (rc != 0) {
                cerr << "The type of updated value doesn't match that of column " << info.attrName << "\n";
                return rc;
            }
        }
    }
    rc = deleteIndex(data, record.getRID());
    if (rc != 0) {
        return -1;
    }
    result = checkData(data);
    if (!result.empty()) {
        RM_Record old_record;
        fileHandle.getRec(record.getRID(), old_record);
        insertIndex(old_record.getData(), record.getRID());
        cerr << std::string(result);
        return -1;
    }
    // todo check here
    insertIndex(data, record.getRID());
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

IX_IndexHandle &Table::getIndexHandler(int index) {
    return *indexHandles[index];
}

