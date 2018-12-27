#ifndef SM_INTERNAL_H
#define SM_INTERNAL_H

#include "sm.h"

#define TABLE_COUNT 21

struct TableList {
	char tables[TABLE_COUNT][MAX_NAME];
	int tableCount;
	void operator = (const TableList &);
};
struct ConstNode {
	int value_i = 0;
    float value_f = 0.0;
	char value_s[MAX_STRING_LEN];
	AttrType attrType = AttrType::NO_ATTR;
	bool is_null = true;
	ConstNode() = default;
	ConstNode(int i);
	ConstNode(float f);
	ConstNode(const char *s);
	void operator = (const ConstNode &);
};
struct ColumnList {
	char columns[MAX_ATTRS][MAX_NAME];
	int columnCount;
	void addColumn(const char* column);
	ColumnList & operator = (const ColumnList &);
};
struct TableCons {
	ConstraintType type;
    ConstNode constNodes[MAX_ATTRS];
	int constSize;
	ColumnList columnList;
	char column_name[MAX_NAME];
	char foreign_table[MAX_NAME];
	char foreign_column[MAX_NAME];
	TableCons & operator = (const TableCons &);
};
struct TableInfo {
	int attrInfoCount;
	AttrInfo attrInfos[MAX_ATTRS];
	int indexedAttrSize;
	int indexedAttr[MAX_ATTRS];
	int tableConsCount;
	TableInfo & operator = (const TableInfo &);
};

ConstNode* getConstNodeFromExpr(Expr *expr);
Expr* getExprFromConstNode(ConstNode *constNode);
void printAttrType(AttrType attrType);
void printConstraintType(ConstraintType constraintType);
void printConstNodeExpr(Expr *expr);
void Debug(const char* file, int line, int err);
IdentList* getIdentListFromColumnList(ColumnList *columnList);
#endif