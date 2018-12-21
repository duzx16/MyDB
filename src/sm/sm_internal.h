#ifndef SM_INTERNAL_H
#define SM_INTERNAL_H

#include "sm.h"

#define MAX_ATTRS 21
#define EXPR_MAX_SIZE 21
struct ExprNode {
	int left, right;
	int value_i = 0;
    float value_f = 0.0;
    bool value_b = false;
	AttributeNode attribute;
	bool is_null = true;
	ArithOp arithOp = ArithOp::NO_OP;
    CompOp compOp = CompOp::NO_OP;
    NodeType nodeType = NodeType::CONST_NODE;
    AttrType attrType = AttrType::NO_ATTR;
	ExprNode() = default;
	// CONST_NODE
	ExprNode(int i);
	ExprNode(float f);
	ExprNode(const char *s);
	// ARITH_NODE
	ExprNode(int left, ArithOp arithOp, int right);
	// COMP_NODE
	ExprNode(int left, CompOp compOp, int right);
	// ATTR_NODE
	ExprNode(AttributeNode attribute);
};
struct ExprTree {
	ExprNode exprs[EXPR_MAX_SIZE];
	int count;
};
struct TableCons {
	ConstraintType type;
    ExprTree exprs[MAX_ATTRS];
	int exprSize;
    IdentList column_list;
    std::string column_name;
    std::string foreign_table;
    std::string foreign_column;
};
struct TableInfo {
	int attrInfoCount;
	AttrInfo attrInfos[MAX_ATTRS];
	int indexedAttrSize;
	int indexedAttr[MAX_ATTRS];
	int tableConsCount;
	TableCons tableCons[MAX_ATTRS];
};

int getExprNode(Expr *expr, int &curSize, ExprNode *exprs);
ExprTree getExprTreeFromExpr(const Expr* expr);
Expr* getExprFromExprTree(ExprTree &exprTree);
Expr* getExprFromExprNode(int id, ExprNode *exprs);
#endif