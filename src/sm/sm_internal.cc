#include "sm_internal.h"
#include <cstdio>
#include <cstring>
#include <cstdlib>

ConstNode::ConstNode(int i) {
	is_null = false;
    value_i = i;
    attrType = AttrType::INT;
}

ConstNode::ConstNode(float f) {
	is_null = false;
    value_f = f;
    attrType = AttrType::FLOAT;
}

ConstNode::ConstNode(const char *s) {
	//printf("ConstNode::ConstNode s = %s\n", s);
	is_null = false;
	int length = strlen(s);
	for (int i = 0; i < length; ++i)
		value_s[i] = s[i];
	value_s[length] = '\0';
	//printf("ConstNode::ConstNode value_s = %s\n", value_s);
	//memset(value_s, 0, sizeof value_s);
	//strcpy(value_s, s);
    attrType = AttrType::STRING;
}

ConstNode* getConstNodeFromExpr(Expr *expr) {
	if (expr->attrType == AttrType::INT) {
		return new ConstNode(expr->value_i);
	}
	else if (expr->attrType == AttrType::FLOAT) {
		return new ConstNode(expr->value_f);
	}
	else if (expr->attrType == AttrType::STRING) {
		return new ConstNode((expr->value_s).c_str());
	}
	else {
		return new ConstNode();
	}
}

Expr* getExprFromConstNode(ConstNode *constNode) {
	if (constNode->attrType == AttrType::INT) {
		return new Expr(constNode->value_i);
	}
	else if (constNode->attrType == AttrType::FLOAT) {
		return new Expr(constNode->value_f);
	}
	else if (constNode->attrType == AttrType::STRING) {
		//printf("getExprFromConstNode value_s src = %s\n", constNode->value_s);
		char s[1010] = {};
		strcat(s, "(");
		strcat(s, constNode->value_s);
		strcat(s, ")");
		assert(strlen(s) == strlen(constNode->value_s) + 2);
		Expr *_ = new Expr(s);
		return _;
	}
	else {
		return new Expr();
	}
}

void printAttrType(AttrType attrType) {
	switch (attrType) {
		case AttrType::INT: { printf("INT"); break; }
		case AttrType::FLOAT: { printf("FLOAT"); break; }
		case AttrType::STRING: { printf("STRING"); break; }
		case AttrType::DATE: { printf("DATE"); break; }
		case AttrType::VARCHAR: { printf("VARCHAR"); break; }
		case AttrType::NO_ATTR: { printf("NO_ATTR"); break; }
		default: {}
	}
}

void printConstraintType(ConstraintType constraintType) {
	switch (constraintType) {
		case ConstraintType::PRIMARY_CONSTRAINT:{ printf("PRIMARY_CONSTRAINT"); break; }
		case ConstraintType::FOREIGN_CONSTRAINT: { printf("FOREIGN_CONSTRAINT"); break; }
		case ConstraintType::CHECK_CONSTRAINT: { printf("CHECK_CONSTRAINT"); break; }
		default: {}
	}
}

void printConstNodeExpr(Expr *expr) {
	switch (expr->attrType) {
		case AttrType::INT:{ printf("%d", expr->value_i); break; }
		case AttrType::FLOAT: { printf("%f", expr->value_f); break; }
		case AttrType::STRING: { printf("'%s'", (expr->value_s).c_str()); break; }
		case AttrType::NO_ATTR: { printf("NULL"); break; }
		default: {}
	}
}
void Debug(const char* file, int line, int err) {
	if (err != 0) {
		printf("Error in file %s, line = %d code = %d\n", file, line, err);
	}
}

void ColumnList::addColumn(const char* column) {
	strcpy(columns[columnCount++], column);
}

IdentList* getIdentListFromColumnList(ColumnList *columnList) {
	IdentList *identList = new IdentList();
	for (int i = 0; i < columnList->columnCount; ++i)
		identList->addIdent(columnList->columns[i]);
	return identList;
}

void TableList::operator = (const TableList &_) {
	memcpy(tables, _.tables, sizeof (tables));
	tableCount = _.tableCount;
}

void ConstNode::operator = (const ConstNode &_) {
	//printf("ConstNode::operator =, in\n");
	value_i = _.value_i;
	value_f = _.value_f;
	memset(value_s, 0, sizeof (value_s));
	int length = strlen(_.value_s);
	for (int i = 0; i < length; ++i)
		value_s[i] = _.value_s[i];
	value_s[length] = '\0';
	//printf("after ConstNode::operator =, value_s = %s\n", value_s);
	//memcpy(value_s, _.value_s, sizeof (value_s));
	attrType = _.attrType;
	is_null = _.is_null;
	//printf("ConstNode::operator =, out\n");
}

void ColumnList::operator = (const ColumnList &_) {
	memcpy(columns, _.columns, sizeof (columns));
	columnCount = _.columnCount;
}

void TableCons::operator = (const TableCons &_) {
	type = _.type;
	constSize = _.constSize;
	for (int i = 0; i < _.constSize; ++i)
		constNodes[i] = _.constNodes[i];
	columnList = _.columnList;
	memcpy(column_name, _.column_name, sizeof (column_name));
	memcpy(foreign_table, _.foreign_table, sizeof (foreign_table));
	memcpy(foreign_column, _.foreign_column, sizeof (foreign_column));
}

void TableInfo::operator = (const TableInfo &_) {
	attrInfoCount = _.attrInfoCount;
	for (int i = 0; i < _.attrInfoCount; ++i)
		attrInfos[i] = _.attrInfos[i];
	indexedAttrSize = _.indexedAttrSize;
	for (int i = 0; i < indexedAttrSize; ++i)
		indexedAttr[i] = _.indexedAttr[i];
	tableConsCount = _.tableConsCount;
}