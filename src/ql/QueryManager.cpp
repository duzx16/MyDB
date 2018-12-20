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
    return 0;
}

int QL_Manager::exeInsert(std::string relationName, ConstValueLists *insertValueTree) {
    return 0;
}

int QL_Manager::exeUpdate(std::string relationName, SetClauseList *setClauses, Expr *whereClause) {
    try {
        bindAttribute(whereClause, relationName);
        for (auto &it: setClauses->clauses) {
            bindAttribute(it.second, relationName);
        }
    } catch (AttrBindException &exception) {
        printException(exception);
        return -1;
    }

    return 0;
}

int QL_Manager::exeDelete(std::string relationName, Expr *whereClause) {
    try {
        bindAttribute(whereClause, relationName);
    } catch (AttrBindException &exception) {
        printException(exception);
        return -1;
    }
    std::vector<RID> toBeDeleted;
    iterateRecords(relationName, whereClause,
                   [&toBeDeleted](RM_FileHandle &fileHandle, const RM_Record &record) -> void {
                       toBeDeleted.push_back(record.getRID());
                   });
    return 0;
}

void QL_Manager::bindAttribute(Expr *expr, const std::string &relationName) {
    ColumnDecsList columns;
    TableConstraintList constraintList;
    expr->connect_attribute(std::vector<std::string>{relationName}, std::vector<ColumnDecsList *>{&columns},
                            std::vector<TableConstraintList *>{&constraintList});
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

void QL_Manager::iterateRecords(std::string relationName, Expr *condition, QL_Manager::CallbackFunc callback) {
    RecordManager &rm = RecordManager::getInstance();
    RM_FileHandle fileHandle;
    rm.openFile(std::move(relationName), fileHandle);
    RM_FileScan fileScan;
    fileScan.openScan(fileHandle, condition);
    while (true) {
        RM_Record record;
        int rc = fileScan.getNextRec(record);
        if (rc) {
            break;
        }
        callback(fileHandle, record);
    }
}
