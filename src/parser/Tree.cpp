//
// Created by Zhengxiao Du on 2018/11/24.
//

#include "Tree.h"
#include "../ql/QueryManager.h"
#include <utility>
#include <cstdlib>
#include <cstring>
#include "../sm/sm.h"

using std::string;
using std::vector;

#include <stdarg.h>

static void MyDebugPrintf(const char *format, ...) {
    va_list argPtr;
    int count;

    va_start(argPtr, format);                  /*  获取可变参数列表  */
    fflush(stdout);                            /*  强制刷新输出缓冲区  */
    count = vfprintf(stderr, format, argPtr);  /*  将信息输出到标准出错流设备  */
    va_end(argPtr);                            /*  可变参数列表结束  */
}

#ifdef DEBUG   /*  如果定义了插桩信息宏，就将调试信息指向调试函数  */
#define DebugPrintf  MyDebugPrintf

#else           /*  如果未定义插桩信息宏，那么就将调试信息指向空NOP  */
#define DebugPrintf

#endif

Tree *Tree::tree = nullptr;

/* SelectTree */
Select::Select(IdentList *relations, Expr *whereClause) {
    this->attributes = nullptr;
    this->relations = relations;
    this->whereClause = whereClause;
    this->groupAttrName = string();
}

Select::Select(AttributeList *attributes,
               IdentList *relations,
               Expr *whereClause,
               const char *groupAttrName) {
    this->attributes = attributes;
    this->relations = relations;
    this->whereClause = whereClause;
    this->groupAttrName = string(groupAttrName);
}

Select::~Select() {
    delete attributes;
    delete relations;
    delete whereClause;
}

void Select::visit() {
    DebugPrintf("select\n");
    bool statistics = false;
    for (const auto &attribute: this->attributes->attributes) {
        if (attribute->aggregationType != AggregationType::T_NONE) {
            statistics = true;
            break;
        }
    }
    if (statistics) {
        for (const auto &attribute: this->attributes->attributes) {
            if (attribute->aggregationType == AggregationType::T_NONE) {
                if (not(attribute->attribute == groupAttrName)) {
                    cerr << "Can't select column " + attribute->attribute + " along with aggregating function\n";
                    return;
                }
            }
        }
    }
    QL_Manager::getInstance().exeSelect(attributes, relations, whereClause, groupAttrName);
}


/* InsertTree */
Insert::Insert(const char *relationName, IdentList *columnList, ConstValueLists *insertValueTree) {
    this->relationName = string(relationName);
    this->columnList = columnList;
    this->insertValueTree = insertValueTree;
}

Insert::~Insert() {
    delete insertValueTree;
}

void Insert::visit() {
    DebugPrintf("insert\n");
    QL_Manager::getInstance().exeInsert(relationName, columnList, insertValueTree);
}

/* UpdateTree */
Update::Update(string relationName,
               SetClauseList *setClauseList,
               Expr *whereClause) {
    this->relationName = std::move(relationName);
    this->setClauses = setClauseList;
    this->whereClause = whereClause;
}

Update::~Update() {
    delete setClauses;
    delete whereClause;
}

void Update::visit() {
    DebugPrintf("update\n");
    QL_Manager::getInstance().exeUpdate(relationName, setClauses, whereClause);
}

/* DeleteTree */
Delete::Delete(const char *relationName, Expr *whereClause) {
    this->relationName = string(relationName);
    this->whereClause = whereClause;
}

Delete::~Delete() {
    delete whereClause;
}

void Delete::visit() {
    DebugPrintf("delete\n");
    QL_Manager::getInstance().exeDelete(relationName, whereClause);
}

/* CreateDatabaseTree */
CreateDatabase::CreateDatabase(const char *dbName) {
    this->dbName = string(dbName);
}

CreateDatabase::~CreateDatabase() = default;

void CreateDatabase::visit() {
    DebugPrintf("create database %s\n", dbName.c_str());
    (SM_Manager::getInstance())->CreateDb(dbName.c_str());
}

/* CreateTableTree */
CreateTable::CreateTable(const char *tableName, ColumnDecsList *columns, TableConstraintList *tableConstraints) {
    this->tableName = string(tableName);
    this->columns = columns;
    this->tableConstraints = tableConstraints;
}

CreateTable::~CreateTable() {
    delete columns;
    delete tableConstraints;
}

void CreateTable::visit() {
    DebugPrintf("create table %s\n", tableName.c_str());
    int rc = (SM_Manager::getInstance())->CreateTable(tableName.c_str(), columns, tableConstraints);
    //todo rc is not correct
    if (rc != 0) {
        fprintf(stderr, "Create table error\n");
        return;
    }
    if (this->tableConstraints != nullptr) {
        for (const auto &constraint: this->tableConstraints->tbDecs) {
            if (constraint->type == ConstraintType::PRIMARY_CONSTRAINT) {
                for (int i = 0; i < this->columns->columns.size(); ++i) {
                    if (columns->columns[i]->columnName == constraint->column_list->idents[0]) {
                        IX_Manager::getInstance().CreateIndex(tableName.c_str(), i, columns->columns[i]->type,
                                                              columns->columns.size());
                    }
                }
            }
        }
    }
    unsigned recordSize = 0;
    for (const auto &it: columns->columns) {
        recordSize += it->size + 1;
    }
    rc = RecordManager::getInstance().createFile(tableName, recordSize);
    DebugPrintf("create table %s end\n", tableName.c_str());
}


/* CreateIndexTree */
CreateIndex::CreateIndex(const char *relName, AttributeNode *attr) {
    this->relName = string(relName);
    this->attribute = attr;
}

CreateIndex::~CreateIndex() {
    delete attribute;
}

void CreateIndex::visit() {
    DebugPrintf("create index %s\n", relName.c_str());
    int rc = SM_Manager::getInstance()->CreateIndex(relName.c_str(), attribute->attribute.c_str());
    if (rc == SM_INDEX_EXISTS) {
        fprintf(stderr, "The index of attribute %s already exists\n", attribute->attribute.c_str());
    } else if (rc == SM_INDEX_NOTEXIST) {
        fprintf(stderr, "The index of attribute %s doesn't exist\n", attribute->attribute.c_str());
    } else if (rc != 0) {
        fprintf(stderr, "Unknown error\n");
    }
}


DropIndex::DropIndex(const char *relName, AttributeNode *attr) {
    this->tableName = string(relName);
    this->attribute = attr;
}

DropIndex::~DropIndex() {
    delete attribute;
}

void DropIndex::visit() {
    DebugPrintf("drop index %s\n", tableName.c_str());
    int rc = SM_Manager::getInstance()->DropIndex(tableName.c_str(), attribute->attribute.c_str());
    if (rc == SM_INDEX_NOTEXIST) {
        fprintf(stderr, "The index of attribute %s doesn't exist\n", attribute->attribute.c_str());
    } else if (rc != 0) {
        fprintf(stderr, "Unknown error\n");
    }
}


/* DropDatabase */
DropDatabase::DropDatabase(const char *dbName) {
    this->dbName = string(dbName);
}

DropDatabase::~DropDatabase() = default;

void DropDatabase::visit() {
    SM_Manager::getInstance()->DropDb(dbName.c_str());
}

/* DropTableTree */
DropTable::DropTable(const char *tableName) {
    this->tableName = string(tableName);
}

DropTable::~DropTable() = default;

void DropTable::visit() {
    DebugPrintf("drop foreign_table %s\n", tableName.c_str());
    int rc = SM_Manager::getInstance()->DropTable(tableName.c_str());
    if (rc == 0) {
        rc = RecordManager::getInstance().destroyFile(tableName);
        if (rc != 0) {
            cerr << "The record file doesn't exist\n";
        }
    } else {
        cerr << "Delete error, the table doesn't exist\n";
    }

}

/* ColumnNode */
ColumnNode::ColumnNode(const char *columnName, AttrType type, int length,
                       int columnFlag) {
    this->columnName = string(columnName);
    this->type = type;
    this->length = length;
    this->columnFlag = columnFlag;
    switch (type) {
        case AttrType::DATE:
        case AttrType::INT:
            this->size = sizeof(int);
            break;
        case AttrType::FLOAT:
            this->size = sizeof(float);
            break;
        case AttrType::VARCHAR:
        case AttrType::STRING:
            this->size = length + 1;
            break;
        case AttrType::BOOL:
        case AttrType::NO_ATTR:
            break;
    }
}

ColumnNode::~ColumnNode() = default;

AttrInfo ColumnNode::getAttrInfo() const {
    AttrInfo attrInfo{};
    strncpy(attrInfo.attrName, columnName.c_str(), MAX_NAME);
    attrInfo.attrType = type;
    attrInfo.attrSize = size;
    attrInfo.attrLength = length;
    attrInfo.columnFlag = columnFlag;
    return attrInfo;
}


/* AttributeList */
AttributeList::AttributeList() = default;

AttributeList::~AttributeList() {
    for (const auto &attribute : attributes)
        delete attribute;
}

void AttributeList::addAttribute(AttributeNode *attribute) {
    attributes.push_back(attribute);
}

vector<AttributeNode::AttributeDescriptor> AttributeList::getDescriptors() const {
    vector<AttributeNode::AttributeDescriptor> attrs;
    for (auto attribute : attributes)
        attrs.push_back(attribute->getDescriptor());
    return attrs;
}

/* AttributeTree */
AttributeNode::AttributeNode(const char *relationName, const char *attributeName, AggregationType aggregationType) {
    this->table = string(relationName);
    this->attribute = string(attributeName);
    this->aggregationType = aggregationType;
}

AttributeNode::AttributeNode(const char *attributeName, AggregationType aggregationType) {
    this->attribute = string(attributeName);
    this->aggregationType = aggregationType;
}

AttributeNode::AttributeDescriptor AttributeNode::getDescriptor() const {
    return AttributeDescriptor(table, attribute, aggregationType);
}

bool AttributeNode::operator==(const AttributeNode &attribute) const {
    return this->table == attribute.table &&
           this->attribute == attribute.attribute &&
           this->aggregationType == attribute.aggregationType;
}

AttributeNode::~AttributeNode() = default;


/* ConstValuesTree */
ConstValueList::ConstValueList() = default;

ConstValueList::~ConstValueList() {
    for (const auto &constValue: constValues)
        delete constValue;
}

void ConstValueList::addConstValue(Expr *constValue) {
    constValues.push_back(constValue);
}

UseDatabase::UseDatabase(const char *dbName) {
    this->dbName = dbName;
}

void UseDatabase::visit() {
    DebugPrintf("Use %s\n", dbName.c_str());
//    SystemManager::instance()->openDB(dbName.c_str());
    (SM_Manager::getInstance())->OpenDb(dbName.c_str());
}

DescTable::DescTable(const char *relName) {
    this->tableName = string(relName);
}

DescTable::~DescTable() = default;

void DescTable::visit() {
    DebugPrintf("desc foreign_table %s\n", tableName.c_str());
    if (tableName.length() == 0)
        (SM_Manager::getInstance())->Help();
    else
        (SM_Manager::getInstance())->Help(tableName.c_str());
    DebugPrintf("desc foreign_table end\n");
}

ConstValueLists::ConstValueLists() = default;

ConstValueLists::~ConstValueLists() {
    for (auto v : values) {
        delete v;
    }
}

void ConstValueLists::addConstValues(ConstValueList *constValuesTree) {
    values.push_back(constValuesTree);
}

SetClauseList::SetClauseList() = default;

void SetClauseList::addSetClause(AttributeNode *attr, Expr *value) {
    this->clauses.emplace_back(attr, value);
}

IdentList::IdentList() = default;

void IdentList::addIdent(const char *ident) {
    this->idents.emplace_back(ident);
}

IdentList::~IdentList() = default;


void ShowDatabase::visit() {
    DebugPrintf("show database: %s\n", DBname.c_str());
}

ColumnDecsList::ColumnDecsList() = default;

ColumnDecsList::~ColumnDecsList() {
    for (auto v: columns) {
        delete v;
    }
}

void ColumnDecsList::addColumn(ColumnNode *column) {
    columns.push_back(column);
}

int ColumnDecsList::getColumnCount() {
    return columns.size();
}

AttrInfo *ColumnDecsList::getAttrInfos() {
    if (attrInfos == nullptr) {
        attrInfos = new AttrInfo[columns.size()];
        for (int i = 0; i < columns.size(); ++i) {
            attrInfos[i] = columns[i]->getAttrInfo();
        }
    }
    return attrInfos;
}

void ColumnDecsList::deleteAttrInfos() {
    delete attrInfos;
}

std::pair<AttrInfo, int> ColumnDecsList::findColumn(std::string name) {
    int offset = 0;
    for (const auto &it: columns) {
        if (it->columnName == name) {
            return {it->getAttrInfo(), offset};
        } else {
            offset += it->size;
        }
    }
    return {AttrInfo{}, -1};
}

// Primary key
TableConstraint::TableConstraint(IdentList *column_list) : type(ConstraintType::PRIMARY_CONSTRAINT),
                                                           column_list(column_list) {

}

// Foreign key
TableConstraint::TableConstraint(const char *column_name, const char *table, const char *column) : type(
        ConstraintType::FOREIGN_CONSTRAINT),
                                                                                                   column_name(
                                                                                                           column_name),
                                                                                                   foreign_table(table),
                                                                                                   foreign_column(
                                                                                                           column) {

}

// Check key
TableConstraint::TableConstraint(const char *column_name, ConstValueList *const_values) : type(
        ConstraintType::CHECK_CONSTRAINT), const_values(const_values), column_name(column_name) {

}

TableConstraint::~TableConstraint() {
    delete column_list;
    delete const_values;
}

void TableConstraintList::addTbDec(TableConstraint *dec) {
    tbDecs.push_back(dec);
}

TableConstraintList::~TableConstraintList() {
    for (auto v: tbDecs) {
        delete v;
    }
}
