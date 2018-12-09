//
// Created by Zhengxiao Du on 2018-12-09.
//

#include "Expr.h"

Expr::Expr(int i) {
    is_null = false;
    value_i = i;
    nodeType = NodeType::CONST_NODE;
    attrType = AttrType::INT;
}

Expr::Expr(float f) {
    is_null = false;
    value_f = f;
    nodeType = NodeType::CONST_NODE;
    attrType = AttrType::FLOAT;
}

Expr::Expr(const char *s) {
    is_null = false;
    value_s = std::string(s, 1, strlen(s) - 2);;
    nodeType = NodeType::CONST_NODE;
    attrType = AttrType::STRING;
}

Expr::~Expr() {
    delete left;
    delete right;
}

Expr::Expr() = default;

Expr::Expr(Expr *left, ArithOp op, Expr *right) {
    is_null = false;
    this->left = left;
    this->right = right;
    this->arithOp = op;
    this->nodeType = NodeType::ARITH_NODE;
}

Expr::Expr(Expr *left, CompOp op, Expr *right) {
    is_null = false;
    this->left = left;
    this->right = right;
    this->compOp = op;
    this->nodeType = NodeType::COMP_NODE;
}

Expr::Expr(AttributeNode *attributeNode) {
    is_null = false;
    this->attribute = attributeNode;
    this->nodeType = NodeType::ATTR_NODE;
}

