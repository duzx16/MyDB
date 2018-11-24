#include <utility>

//
// Created by Zhengxiao Du on 11/24/18.
//

#ifndef DATABASE_TREE_H
#define DATABASE_TREE_H

#include <string>
#include <vector>
#include "../Constants.h"

class Tree;
//Sys Statement
class ShowDatabases;
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
class WhereClauseTree;
class ConditionsTree;
class Comparison;
class ConstValueList;
class ConstValueNode;
class ConstValueLists;
class SetClauseList;

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


class ShowDatabases: public Tree {
public:
    ShowDatabases();
    void visit() override;
};


class Select : public Tree {
public:
    Select(AttributeList *attributes,
               IdentList *relations,
               WhereClauseTree *whereClause,
               const char* groupAttrName = "");
    Select(IdentList *relations, WhereClauseTree *whereClause);

    ~Select() override;
    void visit() override;
private:
    AttributeList *attributes;
    IdentList *relations;
    WhereClauseTree *whereClause;
    std::string groupAttrName;
};


class Insert : public Tree {
public:
    Insert(const char *relationName, ConstValueLists* insertValueTree);

    ~Insert() override;
    void visit() override;
private:
    std::string relationName;
    ConstValueLists* insertValueTree;
};


class Update : public Tree {
public:
    Update(std::string relationName,
               SetClauseList *setClauses,
               WhereClauseTree *whereClause);

    ~Update() override;
    void visit() override;
private:
    std::string relationName;
    SetClauseList *setClauses;
    WhereClauseTree *whereClause;
};


class Delete : public Tree {
public:
    Delete(const char *relationName, WhereClauseTree *whereClause);

    ~Delete() override;
    void visit() override;
private:
    std::string relationName;
    WhereClauseTree *whereClause;
};


class UseDatabase : public Tree {
public:
    explicit UseDatabase(const char *dbName);
    void visit() override;
private:
    std::string dbName;
};


class CreateDatabase : public Tree {
public:
    explicit CreateDatabase(const char *dbName);

    ~CreateDatabase() override;
    void visit() override;
private:
    std::string dbName;
};

class DescTable : public Tree {
public:
    explicit DescTable(const char* relName);

    ~DescTable() override;
    void visit() override;
private:
    std::string relName;
};


class CreateTable : public Tree {
public:
    CreateTable(const char *tableName, ColumnDecsList *columns);

    ~CreateTable() override;
    void visit() override;
private:
    std::string tableName;
    ColumnDecsList *columns;
};


class DropDatabase : public Tree {
public:
    explicit DropDatabase(const char *dbName);

    ~DropDatabase() override;
    void visit() override;
private:
    std::string dbName;
};


class DropTable : public Tree {
public:
    explicit DropTable(const char *tableName);

    ~DropTable() override;
    void visit() override;
private:
    std::string tableName;
};


class CreateIndex: public Tree {
public:
    CreateIndex(const char *relName, AttributeNode *attr);

    ~CreateIndex() override;
    void visit() override;

private:
    std::string relName;
    AttributeNode *attribute;
};


class DropIndex: public Tree {
public:
    DropIndex(const char *relName, AttributeNode *attr);
    ~DropIndex() override;
    void visit() override;

private:
    std::string relName;
    AttributeNode *attribute;
};

class ColumnDecsList : public Tree {
public:
    ColumnDecsList();
    virtual ~ColumnDecsList();
    void addColumn(ColumnNode *);
    bool setPrimaryKey(const char* attr);

    int getColumnCount();
    AttrInfo *getAttrInfos();
    void deleteAttrInfos();
private:
    std::vector<ColumnNode *> columns;
    AttrInfo *attrInfos;
};


class ColumnNode : public Tree {
public:
    ColumnNode(const char *columnName, AttrType type, int size = 4,
               int columnFlag = 0);

    ~ColumnNode() override;

    AttrInfo getAttrInfo();
    friend class ColumnsTree;
private:
    std::string columnName;
    AttrType type;
    int size;
    int isPrimaryKey;
    int columnFlag;
};


class AttributeNode : public Tree {
public:
    AttributeNode(const char *relationName, const char *attributeName,
                  AggregationType aggregationType = AggregationType::T_NONE);
    explicit AttributeNode(const char *attributeName, AggregationType aggregationType = AggregationType::T_NONE);

    ~AttributeNode() override;

    bool operator ==(const AttributeNode &attribute) const;

    struct AttributeDescriptor {
        std::string relName;
        std::string attrName;
        AggregationType aggregationType;
        explicit AttributeDescriptor(std::string relName = "",
                            std::string attrName = "",
                            AggregationType aggregationType = AggregationType::T_NONE) :
                relName(std::move(relName)), attrName(attrName), aggregationType(aggregationType) {}
    };

    AttributeDescriptor getDescriptor() const;
private:
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


class ConstValueNode : public Tree {
public:
    explicit ConstValueNode(int i);
    explicit ConstValueNode(float f);
    explicit ConstValueNode(const char *s);

    ~ConstValueNode() override;

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


class Comparison : public Tree {
public:
    explicit Comparison(AttributeNode *attribute);   // is null
    Comparison(AttributeNode *attribute, CompOp op, ConstValueNode *constValue);
    Comparison(AttributeNode *attribute, CompOp op, AttributeNode *attribute2);

    ~Comparison() override;

    struct ComparisonDescriptor {
        AttributeNode::AttributeDescriptor attr;
        CompOp op;
        AttrValue val;
        AttributeNode::AttributeDescriptor attr2;
        bool isAttrCmp;
    };

    ComparisonDescriptor getDescriptor();

private:
    ConstValueNode *constValue;
    AttributeNode *attribute;
    AttributeNode *attribute2;
    CompOp op;
    bool isAttrCmp;
};


class IdentList : public Tree {
public:
    IdentList();

    ~IdentList() override;
    void addIdent(const char *ident);
private:
    std::vector<std::string> idents;
};


class WhereClauseTree : public Tree {
public:
    explicit WhereClauseTree(ConditionsTree *conditions);
    WhereClauseTree();

    ~WhereClauseTree() override;

    std::vector<Comparison::ComparisonDescriptor> getComparision();

private:
    ConditionsTree *conditions;
};


class ConditionsTree : public Tree {
public:
    ConditionsTree();

    ~ConditionsTree() override;
    void addComparison(Comparison *comparison);
    std::vector<Comparison::ComparisonDescriptor> getComparisions();
private:
    std::vector<Comparison *> comparisons;
};


class ConstValueList : public Tree {
public:
    ConstValueList();

    ~ConstValueList() override;
    void addConstValue(ConstValueNode *constValue);

    std::vector<AttrValue> getConstValues();
private:
    std::vector<ConstValueNode *> constValues;
};

class ConstValueLists : public Tree {
public:
    explicit ConstValueLists();

    ~ConstValueLists() override;
    void addConstValues(ConstValueList* constValuesTree);
    std::vector<ConstValueList*> values;
};

class SetClauseList: public Tree {
public:
    SetClauseList();
    void addSetClause(AttributeNode *, ConstValueNode *);
private:
    std::vector<std::pair<AttributeNode *, ConstValueNode *>> clauses;
};

#endif //DATABASE_TREE_H
