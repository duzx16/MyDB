//
// Created by Zhengxiao Du on 2018-12-09.
//

#ifndef MYDB_QL_MANAGER_H
#define MYDB_QL_MANAGER_H

#include "../parser/Tree.h"
#include "../rm/RM_Record.h"
#include "../rm/RM_FileHandle.h"
#include "../sm/sm.h"
#include "Table.h"
#include "../Constants.h"
#include <functional>
#include <memory>

class QL_Manager {
private:
    SM_Manager sm;

    std::vector<RM_Record> recordCaches;

    using CallbackFunc = std::function<void(const RM_Record &)>;

    using tableListIter =  std::vector<std::unique_ptr<Table>>::iterator;

    void printException(const AttrBindException &exception);

    int iterateTables(Table &table, Expr *condition, CallbackFunc callback);

    int iterateTables(tableListIter begin, tableListIter end, Expr *condition, CallbackFunc callback);

    void bindAttribute(Expr *expr, const std::vector<std::unique_ptr<Table>> &tables);

    int openTables(const std::vector<std::string> &tableNames, std::vector<std::unique_ptr<Table>> &tables);

    int whereBindCheck(Expr *expr, std::vector<std::unique_ptr<Table>> &tables);

public:
    int exeSelect(AttributeList *attributes, IdentList *relations, Expr *whereClause, const std::string &grouAttrName);

    int exeInsert(std::string relationName, IdentList *columnList, ConstValueLists *insertValueTree);

    int exeUpdate(std::string relationName, SetClauseList *setClauses, Expr *whereClause);

    int exeDelete(std::string relationName, Expr *whereClause);

    static QL_Manager &getInstance();
};

#define QL_TABLE_FAIL (START_QL_WARN + 1)
#define QL_TYPE_CHECK (START_QL_WARN + 2)

#endif //MYDB_QL_MANAGER_H
