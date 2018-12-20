#include <utility>

#include <utility>

#include <utility>

//
// Created by Zhengxiao Du on 11/24/18.
//

#ifndef DATABASE_TREE_H
#define DATABASE_TREE_H

#include <string>
#include <vector>
#include "../Constants.h"
#include "Expr.h"

class Tree;

//Sys Statement
class ShowDatabase;

// Table Statement
class Select;

class Insert;

class Update;

class Delete;

class CreateTable;

class DropTable;

// Database statement
class CreateDatabase;

class UseDatabase;

class DropDatabase;

// Index Statement
class CreateIndex;

class DropIndex;

// DataType
class ColumnDecsList;

class ColumnNode;

class AttributeList;

class AttributeNode;

class IdentList;

class ConstValueList;

class ConstValueLists;

class SetClauseList;

class TableConstraint;

class TableConstraintList;

class Tree {
public:
    virtual void visit() {
        assert(false);
    }

    virtual ~Tree() = default;

    static void setInstance(Tree *t) {
        delete tree;

        tree = t;
    }

    static void run() {
        if (tree != nullptr)
            tree->visit();
    }

    static Tree *tree;
};
//
//struct AttrValue {
//    AttrType type;
//    int i;
//    float f;
//    std::string s;
//    bool isNull;
//
//    // Definitions are in Tree.cpp
//    bool operator==(const AttrValue &val) const;
//
//    bool operator!=(const AttrValue &val) const;
//
//    bool operator>=(const AttrValue &val) const;
//
//    bool operator<=(const AttrValue &val) const;
//
//    bool operator>(const AttrValue &val) const;
//
//    bool operator<(const AttrValue &val) const;
//};


class ShowDatabase : public Tree {
public:
    explicit ShowDatabase(std::string DBname) : DBname(std::move(DBname)) {};

    void visit() override;

    std::string DBname;
};


class Select : public Tree {
public:
    Select(AttributeList *attributes,
           IdentList *relations,
           Expr *whereClause,
           const char *groupAttrName = "");

    Select(IdentList *relations, Expr *whereClause);

    ~Select() override;

    void visit() override;

    AttributeList *attributes;
    IdentList *relations;
    Expr *whereClause;
    std::string groupAttrName;
};


class Insert : public Tree {
public:
    Insert(const char *relationName, ConstValueLists *insertValueTree);

    ~Insert() override;

    void visit() override;

    std::string relationName;
    ConstValueLists *insertValueTree;
};


class Update : public Tree {
public:
    Update(std::string relationName,
           SetClauseList *setClauses,
           Expr *whereClause);

    ~Update() override;

    void visit() override;

    std::string relationName;
    SetClauseList *setClauses;
    Expr *whereClause;
};


class Delete : public Tree {
public:
    Delete(const char *relationName, Expr *whereClause);

    ~Delete() override;

    void visit() override;

    std::string relationName;
    Expr *whereClause;
};


class UseDatabase : public Tree {
public:
    explicit UseDatabase(const char *dbName);

    void visit() override;

    std::string dbName;
};


class CreateDatabase : public Tree {
public:
    explicit CreateDatabase(const char *dbName);

    ~CreateDatabase() override;

    void visit() override;

    std::string dbName;
};

class DescTable : public Tree {
public:
    explicit DescTable(const char *relName);

    ~DescTable() override;

    void visit() override;

    std::string tableName;
};


class CreateTable : public Tree {
public:
    CreateTable(const char *tableName, ColumnDecsList *columns, TableConstraintList *tableConstraints);

    ~CreateTable() override;

    void visit() override;

    std::string tableName;
    ColumnDecsList *columns;
    TableConstraintList *tableConstraints;
};


class DropDatabase : public Tree {
public:
    explicit DropDatabase(const char *dbName);

    ~DropDatabase() override;

    void visit() override;

    std::string dbName;
};


class DropTable : public Tree {
public:
    explicit DropTable(const char *tableName);

    ~DropTable() override;

    void visit() override;

    std::string tableName;
};


class CreateIndex : public Tree {
public:
    CreateIndex(const char *relName, AttributeNode *attr);

    ~CreateIndex() override;

    void visit() override;

    std::string relName;
    AttributeNode *attribute;
};


class DropIndex : public Tree {
public:
    DropIndex(const char *relName, AttributeNode *attr);

    ~DropIndex() override;

    void visit() override;

    std::string tableName;
    AttributeNode *attribute;
};

class ColumnDecsList : public Tree {
public:
    ColumnDecsList();

    ~ColumnDecsList() override;

    void addColumn(ColumnNode *);

    int getColumnCount();

    AttrInfo *getAttrInfos();

    void deleteAttrInfos();

    std::pair<AttrInfo, int> findColumn(std::string name);

    std::vector<ColumnNode *> columns;
private:
    AttrInfo *attrInfos = nullptr;
};


class ColumnNode : public Tree {
public:
    ColumnNode(const char *columnName, AttrType type, int size = 4,
               int columnFlag = 0);

    ~ColumnNode() override;

    AttrInfo getAttrInfo() const;

    friend class ColumnDecsList;

    std::string columnName;
    AttrType type;
    int size;
    int columnFlag;
};

// For select attribute
class AttributeNode : public Tree {
public:
    AttributeNode(const char *relationName, const char *attributeName,
                  AggregationType aggregationType = AggregationType::T_NONE);

    explicit AttributeNode(const char *attributeName, AggregationType aggregationType = AggregationType::T_NONE);

    ~AttributeNode() override;

    bool operator==(const AttributeNode &attribute) const;

    struct AttributeDescriptor {
        std::string relName;
        std::string attrName;
        AggregationType aggregationType;

        explicit AttributeDescriptor(std::string relName = "",
                                     std::string attrName = "",
                                     AggregationType aggregationType = AggregationType::T_NONE) :
                relName(std::move(relName)), attrName(std::move(attrName)), aggregationType(aggregationType) {}
    };

    AttributeDescriptor getDescriptor() const;

    std::string table;
    std::string attribute;
    AggregationType aggregationType;
};


class AttributeList : public Tree {
public:
    AttributeList();

    ~AttributeList() override;

    void addAttribute(AttributeNode *attribute);

    std::vector<AttributeNode::AttributeDescriptor> getDescriptors() const;

private:
    std::vector<AttributeNode *> attributes;
};


class IdentList : public Tree {
public:
    IdentList();

    ~IdentList() override;

    void addIdent(const char *ident);

    std::vector<std::string> idents;
};

class ConstValueList : public Tree {
public:
    ConstValueList();

    ~ConstValueList() override;

    void addConstValue(Expr *constValue);

//    std::vector<AttrValue> getConstValues();

private:
    std::vector<Expr *> constValues;
};

// For insert values
class ConstValueLists : public Tree {
public:
    explicit ConstValueLists();

    ~ConstValueLists() override;

    void addConstValues(ConstValueList *constValuesTree);

    std::vector<ConstValueList *> values;
};

class SetClauseList : public Tree {
public:
    SetClauseList();

    void addSetClause(AttributeNode *, Expr *);

private:
    std::vector<std::pair<AttributeNode *, Expr *>> clauses;
};

class TableConstraint : public Tree {
public:
    explicit TableConstraint(IdentList *column_list);

    TableConstraint(const char *column_name, const char *table, const char *column);

    TableConstraint(char *column_name, ConstValueList *const_values);

    ~TableConstraint() override;

    ConstraintType type;
    ConstValueList *const_values = nullptr;
    IdentList *column_list = nullptr;
    std::string column_name;
    std::string foreign_table;
    std::string foreign_column;
};

class TableConstraintList : public Tree {
public:
    TableConstraintList() = default;

    ~TableConstraintList() override;

    void addTbDec(TableConstraint *dec);

    std::vector<TableConstraint *> tbDecs;
};


#endif //DATABASE_TREE_H
