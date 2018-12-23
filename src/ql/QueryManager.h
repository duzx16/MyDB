//
// Created by Zhengxiao Du on 2018-12-09.
//

#ifndef MYDB_QL_MANAGER_H
#define MYDB_QL_MANAGER_H

#include "../parser/Tree.h"
#include "../rm/RM_Record.h"
#include "../rm/RM_FileHandle.h"
#include "sm.h"
#include "Table.h"
#include "../Constants.h"
#include <functional>
#include <memory>

class QL_Manager {
private:
    SM_Manager sm;

    using CallbackFunc = std::function<void(RM_FileHandle &, const RM_Record &)>;

    void printException(const AttrBindException &exception);

    int iterateRecords(Table &table, Expr *condition, CallbackFunc callback);

    int iterateRecords(const std::vector<std::unique_ptr<Table>> &tables, Expr *condition, CallbackFunc callback);

    void bindAttribute(Expr *expr, const std::vector<std::unique_ptr<Table>> &tables);

public:
    int exeSelect(AttributeList *attributes, IdentList *relations, Expr *whereClause, std::string groupAttrName);

    int exeInsert(std::string relationName, ConstValueLists *insertValueTree);

    int exeUpdate(std::string relationName, SetClauseList *setClauses, Expr *whereClause);

    int exeDelete(std::string relationName, Expr *whereClause);
};

#define QL_TABLE_FAIL (START_QL_WARN + 1)

#endif //MYDB_QL_MANAGER_H
