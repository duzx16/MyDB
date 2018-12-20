//
// Created by Zhengxiao Du on 2018-12-09.
//

#ifndef MYDB_QL_MANAGER_H
#define MYDB_QL_MANAGER_H

#include "../parser/Tree.h"
#include "../rm/RM_Record.h"
#include "../rm/RM_FileHandle.h"
#include <functional>

class QL_Manager {
private:
    using CallbackFunc = std::function<void(RM_FileHandle &, const RM_Record &)>;

    void bindAttribute(Expr *expr, const std::string &relationName);

    void printException(const AttrBindException &exception);

    void iterateRecords(std::string relationName, Expr *condition, CallbackFunc callback);

public:
    int exeSelect(AttributeList *attributes, IdentList *relations, Expr *whereClause, std::string groupAttrName);

    int exeInsert(std::string relationName, ConstValueLists *insertValueTree);

    int exeUpdate(std::string relationName, SetClauseList *setClauses, Expr *whereClause);

    int exeDelete(std::string relationName, Expr *whereClause);
};


#endif //MYDB_QL_MANAGER_H
