//
// Created by Zhengxiao Du on 2018-12-09.
//

#include "QueryManager.h"

int QL_Manager::Select(AttributeList *attributes, IdentList *relations, Expr *whereClause, std::string groupAttrName) {
    
    return 0;
}

int QL_Manager::Insert(std::string relationName, ConstValueLists *insertValueTree) {
    return 0;
}

int QL_Manager::Update(std::string relationName, SetClauseList *setClauses, Expr *whereClause) {
    return 0;
}

int QL_Manager::Delete(std::string relationName, Expr *whereClause) {
    return 0;
}
