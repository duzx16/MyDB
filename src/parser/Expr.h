//
// Created by Zhengxiao Du on 2018-12-09.
//

#ifndef MYDB_EXPR_H
#define MYDB_EXPR_H

#include "../Constants.h"
#include <string>
#include <vector>
#include <functional>

#define EXPR_NO_SUCH_TABLE -1
#define EXPR_NO_SUCH_ATTRIBUTE -2
#define EXPR_AMBIGUOUS -3

struct AttrBindException {
    std::string table;
    std::string attribute;
    int type;
};

enum class NodeType {
    ARITH_NODE,
    COMP_NODE,
    LOGIC_NODE,
    CONST_NODE,
    ATTR_NODE
};

struct BindAttribute {
    std::string attrName;
    std::string tableName;
    int attrLength{};
    AttrType attrType = AttrType::NO_ATTR;
    bool notNull{};
    bool withIndex;
    int attrOffset{};
};

class AttributeNode;

class ColumnDecsList;

class TableConstraintList;

class Expr {
public:
    Expr();

    explicit Expr(int i);

    explicit Expr(float f);

    explicit Expr(const char *s);

    Expr(Expr *left, ArithOp op, Expr *right = nullptr);

    Expr(Expr *left, CompOp op, Expr *right);

    Expr(Expr *left, LogicOp op, Expr *right);

    explicit Expr(AttributeNode *);

    void postorder(std::function<void(Expr *)> callback, std::function<bool(Expr *)> stop_condition = nullptr);

    void calculate(char *data, const std::string &relationName = "");

    void type_check();

    void convert_to_float();

    void init_calculate(const std::string &tableName = "");


    ~Expr();

    union {
        int i;
        float f;
        bool b;
    } value;

    std::string value_s = "";

    AttributeNode *attribute = nullptr;
    BindAttribute attrInfo;
    bool is_null = true;
    bool calculated = false;


    Expr *left = nullptr, *right = nullptr;
    union {
        ArithOp arith = ArithOp::NO_OP;
        CompOp comp;
        LogicOp logic;
        AttrType constType;
    } oper;

    NodeType nodeType = NodeType::CONST_NODE;
    AttrType dataType;
};


#endif //MYDB_EXPR_H
