#include <utility>

#include <memory>

#include <utility>

//
// Created by Zhengxiao Du on 2018-12-09.
//

#include "QueryManager.h"
#include "../rm/RM_FileHandle.h"
#include "../rm/RecordManager.h"
#include "../rm/RM_FileScan.h"

int
QL_Manager::exeSelect(AttributeList *attributes, IdentList *relations, Expr *whereClause, std::string groupAttrName) {
    std::vector<std::unique_ptr<Table>> tables;
    int rc = 0;
    try {
        for (const auto &ident : relations->idents) {
            auto *table = new Table(ident);
            tables.push_back(std::unique_ptr<Table>(table));
        }
    }
    catch (const std::string &error) {
        cerr << error;
        return QL_TABLE_FAIL;
    }

    try {
        bindAttribute(whereClause, tables);
    } catch (AttrBindException &exception) {
        printException(exception);
        return QL_TABLE_FAIL;
    }
    //todo complete select
    return 0;
}

int QL_Manager::exeInsert(std::string relationName, ConstValueLists *insertValueTree) {
    return 0;
}

int QL_Manager::exeUpdate(std::string relationName, SetClauseList *setClauses, Expr *whereClause) {
    std::vector<std::unique_ptr<Table>> tables;
    try {
        tables.push_back(std::make_unique<Table>(relationName));
    }
    catch (const std::string &error) {
        cerr << error;
        return QL_TABLE_FAIL;
    }
    try {
        bindAttribute(whereClause, tables);
        for (auto &it: setClauses->clauses) {
            bindAttribute(it.second, tables);
        }
    } catch (AttrBindException &exception) {
        printException(exception);
        return QL_TABLE_FAIL;
    }
    iterateRecords(*tables[0], whereClause, [setClauses](RM_FileHandle &fileHandle, const RM_Record &record) -> void {
        // todo complete update
    });
    return 0;
}

int QL_Manager::exeDelete(std::string relationName, Expr *whereClause) {
    std::vector<std::unique_ptr<Table>> tables;
    try {
        tables.push_back(std::make_unique<Table>(relationName));
    }
    catch (const std::string &error) {
        cerr << error;
        return QL_TABLE_FAIL;
    }
    try {
        bindAttribute(whereClause, tables);
    } catch (AttrBindException &exception) {
        printException(exception);
        return QL_TABLE_FAIL;
    }
    std::vector<RID> toBeDeleted;
    iterateRecords(*tables[0], whereClause,
                   [&toBeDeleted](RM_FileHandle &fileHandle, const RM_Record &record) -> void {
                       toBeDeleted.push_back(record.getRID());
                   });
    RM_FileHandle &fileHandle = tables[0]->getFileHandler();
    for (auto &it: toBeDeleted) {
        tables[0]->deleteData(it);
    }
    return 0;
}

void QL_Manager::printException(const AttrBindException &exception) {
    switch (exception.type) {
        case EXPR_NO_SUCH_TABLE:
            fprintf(stderr, "Can't find the table %s\n", exception.table.c_str());
            break;
        case EXPR_NO_SUCH_ATTRIBUTE:
            fprintf(stderr, "Can't find the attribute %s in table %s\n", exception.attribute.c_str(),
                    exception.table.c_str());
            break;
        case EXPR_AMBIGUOUS:
            fprintf(stderr, "Reference to the attribute %s is ambiguous\n", exception.attribute.c_str());
            break;
        default:
            fprintf(stderr, "Unknown error\n");
            break;
    }
}

int QL_Manager::iterateRecords(Table &table, Expr *condition, QL_Manager::CallbackFunc callback) {
    int rc;
    RM_FileScan fileScan;
    fileScan.openScan(table.getFileHandler(), condition);
    while (true) {
        RM_Record record;
        rc = fileScan.getNextRec(record);
        if (rc) {
            break;
        }
        callback(table.getFileHandler(), record);
    }
    return 0;
}

void QL_Manager::bindAttribute(Expr *expr, const std::vector<std::unique_ptr<Table>> &tables) {
    expr->postorder([&tables](Expr *expr1) {
        if (expr1->nodeType == NodeType::ATTR_NODE) {
            if (!expr1->attribute->table.empty()) {
                for (const auto &table: tables) {
                    if (table->tableName == expr1->attribute->table) {
                        int offset = table->getOffset(expr1->attribute->attribute);
                        if (offset > 0) {
                            ColumnNode *column = table->getColumn(expr1->attribute->attribute);
                            expr1->attrInfo.attrType = column->type;
                            expr1->attrInfo.attrOffset = offset;
                            expr1->attrInfo.attrLength = column->size;
                            expr1->attrInfo.tableName = table->tableName;
                            return;
                        } else {
                            throw AttrBindException{"", expr1->attribute->attribute, EXPR_NO_SUCH_ATTRIBUTE};
                        }
                    }
                }
                throw AttrBindException{expr1->attribute->table, "", EXPR_NO_SUCH_TABLE};
            } else {
                expr1->attrInfo.attrOffset = -1;
                for (const auto &table: tables) {
                    if (not expr1->attrInfo.tableName.empty()) {
                        throw AttrBindException{"", expr1->attribute->attribute, EXPR_AMBIGUOUS};
                    } else {
                        int offset = table->getOffset(expr1->attribute->attribute);
                        if (offset > 0) {
                            ColumnNode *column = table->getColumn(expr1->attribute->attribute);
                            expr1->attrInfo.attrType = column->type;
                            expr1->attrInfo.attrOffset = offset;
                            expr1->attrInfo.attrLength = column->size;
                            expr1->attrInfo.tableName = table->tableName;
                        }
                    }
                }
            }
        }
    });
}
