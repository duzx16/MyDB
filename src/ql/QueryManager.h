//
// Created by Zhengxiao Du on 2018-12-09.
//

#ifndef MYDB_QL_MANAGER_H
#define MYDB_QL_MANAGER_H

#include "../parser/Tree.h"

class QL_Manager {
public:
    int Select(AttributeList *attributes, IdentList *relations, Expr *whereClause, std::string groupAttrName);

    int Insert(std::string relationName, ConstValueLists *insertValueTree);

    int Update(std::string relationName, SetClauseList *setClauses, Expr *whereClause);

    int Delete(std::string relationName, Expr *whereClause);
};


#endif //MYDB_QL_MANAGER_H
