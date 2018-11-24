//
// Created by Zhengxiao Du on 11/24/18.
//

#ifndef DATABASE_TREE_H
#define DATABASE_TREE_H

#include <string>
#include <vector>
#include "../Constants.h"

class Tree;
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
class ColumnsTree;
class ColumnTree;
class AttributesTree;
class AttributeTree;
class RelationsTree;
class WhereClauseTree;
class ConditionsTree;
class ComparisonTree;
class ConstValuesTree;
class ConstValueTree;
class InsertValueTree;

class Tree {
public:
    virtual void visit() {
        assert(false);
    }
    virtual ~Tree() = default;
    static void setInstance(Tree *t) {
        if (tree != nullptr) {
            delete tree;
        }
        tree = t;
    }
    static void run() {
        if (tree != nullptr)
            tree->visit();
    }
    static Tree *tree;
};

struct AttrValue {
    AttrType type;
    int i;
    float f;
    std::string s;
    bool isNull;

    // Definitions are in Tree.cpp
    bool operator ==(const AttrValue &val) const;
    bool operator !=(const AttrValue &val) const;
    bool operator >=(const AttrValue &val) const;
    bool operator <=(const AttrValue &val) const;
    bool operator > (const AttrValue &val) const;
    bool operator < (const AttrValue &val) const;
};

class Select : public Tree {
public:
    Select(AttributesTree *attributes,
               RelationsTree *relations,
               WhereClauseTree *whereClause,
               const char* groupAttrName = "");
    Select(RelationsTree *relations, WhereClauseTree *whereClause);
    virtual ~Select();
    void visit();
private:
    AttributesTree *attributes;
    RelationsTree *relations;
    WhereClauseTree *whereClause;
    std::string groupAttrName;
};


class Insert : public Tree {
public:
    Insert(const char *relationName, InsertValueTree* insertValueTree);
    virtual ~Insert();
    void visit();
private:
    std::string relationName;
    InsertValueTree* insertValueTree;
};


class Update : public Tree {
public:
    Update(std::string relationName,
               AttributeTree *attribute,
               ConstValueTree *constValue,
               WhereClauseTree *whereClause);
    virtual ~Update();
    void visit();
private:
    std::string relationName;
    AttributeTree *attribute;
    ConstValueTree *constValue;
    WhereClauseTree *whereClause;
};


class Delete : public Tree {
public:
    Delete(const char *relationName, WhereClauseTree *whereClause);
    virtual ~Delete();
    void visit();
private:
    std::string relationName;
    WhereClauseTree *whereClause;
};


class UseDatabase : public Tree {
public:
    UseDatabase(const char *dbName);
    void visit();
private:
    std::string dbName;
};


class CreateDatabase : public Tree {
public:
    CreateDatabase(const char *dbName);
    virtual ~CreateDatabase();
    void visit();
private:
    std::string dbName;
};

class DescTable : public Tree {
public:
    DescTable(const char* relName);
    virtual ~DescTable();
    void visit();
private:
    std::string relName;
};


class CreateTable : public Tree {
public:
    CreateTable(const char *tableName, ColumnsTree *columns);
    virtual ~CreateTable();
    void visit();
private:
    std::string tableName;
    ColumnsTree *columns;
};


class DropDatabase : public Tree {
public:
    DropDatabase(const char *dbName);
    virtual ~DropDatabase();
    void visit();
private:
    std::string dbName;
};


class DropTable : public Tree {
public:
    DropTable(const char *tableName);
    virtual ~DropTable();
    void visit();
private:
    std::string tableName;
};


class CreateIndex: public Tree {
public:
    CreateIndex(const char *relName, AttributeTree *attr);
    virtual ~CreateIndex();
    void visit();

private:
    std::string relName;
    AttributeTree *attribute;
};


class DropIndex: public Tree {
public:
    DropIndex(const char *relName, AttributeTree *attr);
    virtual ~DropIndex();
    void visit();

private:
    std::string relName;
    AttributeTree *attribute;
};

class ColumnsTree : public Tree {
public:
    ColumnsTree();
    virtual ~ColumnsTree();
    void addColumn(ColumnTree *);
    bool setPrimaryKey(const char* attr);

    int getColumnCount();
    AttrInfo *getAttrInfos();
    void deleteAttrInfos();
private:
    std::vector<ColumnTree *> columns;
    AttrInfo *attrInfos;
};


class ColumnTree : public Tree {
public:
    ColumnTree(const char *columnName, AttrType type, int size = 4,
               int isPrimaryKey = 0, int notNull = 0);
    virtual ~ColumnTree();

    AttrInfo getAttrInfo();
    void setNotNull(int notNull);
    friend class ColumnsTree;
private:
    std::string columnName;
    AttrType type;
    int size;
    int isPrimaryKey;
    int notNull;
};


class AttributeTree : public Tree {
public:
    AttributeTree(const char *relationName, const char *attributeName,
                  AggregationType aggregationType = AggregationType::T_NONE);
    AttributeTree(const char *attributeName, AggregationType aggregationType = AggregationType::T_NONE);
    virtual ~AttributeTree();

    bool operator ==(const AttributeTree &attribute) const;

    struct AttributeDescriptor {
        std::string relName;
        std::string attrName;
        AggregationType aggregationType;
        AttributeDescriptor(std::string relName = "",
                            std::string attrName = "",
                            AggregationType aggregationType = AggregationType::T_NONE) :
                relName(relName), attrName(attrName), aggregationType(aggregationType) {}
    };

    AttributeDescriptor getDescriptor() const;
private:
    std::string relation;
    std::string attribute;
    AggregationType aggregationType;
};


class AttributesTree : public Tree {
public:
    AttributesTree();
    virtual ~AttributesTree();
    void addAttribute(AttributeTree *attribute);
    std::vector<AttributeTree::AttributeDescriptor> getDescriptors() const;
private:
    std::vector<AttributeTree *> attributes;
};


class ConstValueTree : public Tree {
public:
    ConstValueTree(int i);
    ConstValueTree(float f);
    ConstValueTree(const char *s);
    virtual ~ConstValueTree();

    AttrValue getDescriptor();
    void setNull() {isNull = true;}

    friend class ComparisonTree;
private:
    int i;
    float f;
    std::string s;
    AttrType type;
    bool isNull;
};


class ComparisonTree : public Tree {
public:
    ComparisonTree(AttributeTree *attribute);   // is null
    ComparisonTree(AttributeTree *attribute, CompOp op, ConstValueTree *constValue);
    ComparisonTree(AttributeTree *attribute, CompOp op, AttributeTree *attribute2);
    virtual ~ComparisonTree();

    struct ComparisonDescriptor {
        AttributeTree::AttributeDescriptor attr;
        CompOp op;
        AttrValue val;
        AttributeTree::AttributeDescriptor attr2;
        bool isAttrCmp;
    };

    ComparisonDescriptor getDescriptor();

private:
    ConstValueTree *constValue;
    AttributeTree *attribute;
    AttributeTree *attribute2;
    CompOp op;
    bool isAttrCmp;
};


class RelationsTree : public Tree {
public:
    RelationsTree();
    virtual ~RelationsTree();
    void addRelation(const char *relation);
    std::vector<std::string> getRelations();
private:
    std::vector<std::string> relations;
};


class WhereClauseTree : public Tree {
public:
    WhereClauseTree(ConditionsTree *conditions);
    WhereClauseTree();
    virtual ~WhereClauseTree();

    std::vector<ComparisonTree::ComparisonDescriptor> getComparision();

private:
    ConditionsTree *conditions;
};


class ConditionsTree : public Tree {
public:
    ConditionsTree();
    virtual ~ConditionsTree();
    void addComparison(ComparisonTree *comparison);
    std::vector<ComparisonTree::ComparisonDescriptor> getComparisions();
private:
    std::vector<ComparisonTree *> comparisons;
};


class ConstValuesTree : public Tree {
public:
    ConstValuesTree();
    virtual ~ConstValuesTree();
    void addConstValue(ConstValueTree *constValue);

    std::vector<AttrValue> getConstValues();
private:
    std::vector<ConstValueTree *> constValues;
};

class InsertValueTree : public Tree {
public:
    InsertValueTree(ConstValuesTree *constValues);
    virtual ~InsertValueTree();
    void addConstValues(ConstValuesTree* constValuesTree);
    std::vector<ConstValuesTree*> values;
};

class CheckTree : public Tree {
public:
    CheckTree();
    virtual ~CheckTree();
};

class ChecksTree : public Tree {
public:
    ChecksTree();
    virtual ~ChecksTree();
    void addCheckTree(CheckTree* tree);
    std::vector<CheckTree*> checks;
};

#endif //DATABASE_TREE_H
