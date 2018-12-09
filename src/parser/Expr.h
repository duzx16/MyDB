//
// Created by Zhengxiao Du on 2018-12-09.
//

#ifndef MYDB_EXPR_H
#define MYDB_EXPR_H

#include "../Constants.h"
#include <string>

enum class NodeType {
    ARITH_NODE,
    COMP_NODE,
    CONST_NODE,
    ATTR_NODE
};

class AttributeNode;

class Expr {
public:
    Expr();

    explicit Expr(int i);

    explicit Expr(float f);

    explicit Expr(const char *s);

    Expr(Expr *left, ArithOp op, Expr *right = nullptr);

    Expr(Expr *left, CompOp op, Expr *right);

    explicit Expr(AttributeNode *);

    ~Expr();

    std::string value_s = "";
    int value_i = 0;
    float value_f = 0.0;
    bool value_b = false;
    AttributeNode *attribute = nullptr;
    bool is_null = true;


    Expr *left = nullptr, *right = nullptr;
    ArithOp arithOp = ArithOp::NO_OP;
    CompOp compOp = CompOp::NO_OP;
    NodeType nodeType = NodeType::CONST_NODE;
    AttrType attrType = AttrType::NO_ATTR;
};


#endif //MYDB_EXPR_H
