//
// Created by Zhengxiao Du on 2018-12-09.
//

#include "Expr.h"
#include "Tree.h"
#include <regex>

Expr::Expr(int i) {
    is_null = false;
    value.i = i;
    nodeType = NodeType::CONST_NODE;
    oper.attr = AttrType::INT;
}

Expr::Expr(float f) {
    is_null = false;
    value.f = f;
    nodeType = NodeType::CONST_NODE;
    oper.attr = AttrType::FLOAT;
}

Expr::Expr(const char *s) {
    is_null = false;
    value_s = std::string(s, 1, strlen(s) - 2);;
    nodeType = NodeType::CONST_NODE;
    oper.attr = AttrType::STRING;
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
    this->oper.arith = op;
    this->nodeType = NodeType::ARITH_NODE;
}

Expr::Expr(Expr *left, CompOp op, Expr *right) {
    is_null = false;
    this->left = left;
    this->right = right;
    this->oper.comp = op;
    this->nodeType = NodeType::COMP_NODE;
}

Expr::Expr(Expr *left, LogicOp op, Expr *right) {
    is_null = false;
    this->left = left;
    this->right = right;
    this->oper.logic = op;
    this->nodeType = NodeType::LOGIC_NODE;
}

Expr::Expr(AttributeNode *attributeNode) {
    is_null = false;
    this->attribute = attributeNode;
    this->nodeType = NodeType::ATTR_NODE;
}

template<typename T>
bool comp_function(const T &a, const T &b, CompOp compOp) {
    switch (compOp) {
        case CompOp::EQ_OP:
            return a == b;
        case CompOp::GE_OP:
            return not(a < b);
        case CompOp::GT_OP:
            return not(a < b) and not(a == b);
        case CompOp::LE_OP:
            return a < b or a == b;
        case CompOp::LT_OP:
            return a < b;
        case CompOp::NE_OP:
            return not(a == b);
        case CompOp::NO_OP:
            return true;
        case CompOp::IS_OP:
            // todo implement this
            break;
        default:
            return false;
    }
    return false;
}

bool comp_function(const std::string &a, const std::string &b, CompOp compOp) {
    if (compOp == CompOp::LIKE_OP) {
        std::string regexStr;
        char status = 'A';
        for (size_t i = 0; i < b.length(); i++) {
            if (status == 'A') {
                // common status
                if (b[i] == '\\') {
                    status = 'B';
                } else if (b[i] == '[') {
                    regexStr += "[";
                    status = 'C';
                } else if (b[i] == '%') {
                    regexStr += ".*";
                } else if (b[i] == '_') {
                    regexStr += ".";
                } else {
                    regexStr += b[i];
                }
            } else if (status == 'B') {
                // after '\'
                if (b[i] == '%' || b[i] == '_' || b[i] == '!') {
                    regexStr += b[i];
                } else {
                    regexStr += "\\";
                    regexStr += b[i];
                }
                status = 'A';
            } else {
                // after '[' inside []
                if (b[i] == '!') {
                    regexStr += "^";
                } else {
                    regexStr += b[i];
                }
                status = 'A';
            }
        }
        std::regex reg(regexStr);
        return std::regex_match(a, reg);
    } else {
        switch (compOp) {
            case CompOp::EQ_OP:
                return a == b;
            case CompOp::GE_OP:
                return a >= b;
            case CompOp::GT_OP:
                return a > b;
            case CompOp::LE_OP:
                return a <= b;
            case CompOp::LT_OP:
                return a < b;
            case CompOp::NE_OP:
                return a != b;
            case CompOp::NO_OP:
                return true;
            case CompOp::IS_OP:
                // todo implement this
                break;
            default:
                return false;
        }
    }
    return false;
}

template<typename T>
T arith_function(T a, T b, ArithOp arithOp) {
    switch (arithOp) {
        case ArithOp::ADD_OP:
            return a + b;
        case ArithOp::SUB_OP:
            return a - b;
        case ArithOp::MUL_OP:
            return a * b;
        case ArithOp::DIV_OP:
            return a / b;
        case ArithOp::MINUS_OP:
            return -a;
        case ArithOp::NO_OP:
            break;
    }
    return T{};
}

bool logic_function(bool a, bool b, LogicOp logicOp) {
    switch (logicOp) {
        case LogicOp::AND_OP:
            return a and b;
        case LogicOp::OR_OP:
            return a or b;
        case LogicOp::NO_OP:
            break;
    }
    return false;
}


void Expr::calculate(char *data) {
    if (left != nullptr) {
        left->calculate(data);
    }
    if (right != nullptr) {
        right->calculate(data);
    }
    switch (this->nodeType) {
        case NodeType::ARITH_NODE:
            switch (this->dataType) {
                case AttrType::INT:
                    if (this->oper.arith == ArithOp::MINUS_OP) {
                        this->value.i = arith_function(left->value.i, left->value.i, this->oper.arith);
                    } else {
                        this->value.i = arith_function(left->value.i, right->value.i, this->oper.arith);
                    }
                    break;
                case AttrType::FLOAT:
                    if (this->oper.arith == ArithOp::MINUS_OP) {
                        this->value.f = arith_function(left->value.f, left->value.f, this->oper.arith);
                    } else {
                        this->value.f = arith_function(left->value.f, right->value.f, this->oper.arith);
                    }
                    break;
                case AttrType::STRING:
                case AttrType::DATE:
                case AttrType::VARCHAR:
                case AttrType::NO_ATTR:
                    break;
                case AttrType::BOOL:
                    break;
            }
            break;
        case NodeType::COMP_NODE:
            switch (left->dataType) {
                case AttrType::INT:
                case AttrType::DATE:
                    this->value.b = comp_function(left->value.i, right->value.i, oper.comp);
                    break;
                case AttrType::FLOAT:
                    this->value.b = comp_function(left->value.f, right->value.f, oper.comp);
                    break;
                case AttrType::STRING:
                    this->value.b = comp_function(left->value_s, right->value_s, oper.comp);
                    break;
                case AttrType::VARCHAR:
                case AttrType::NO_ATTR:
                    break;
                case AttrType::BOOL:
                    break;
            }
            break;
        case NodeType::LOGIC_NODE:
            this->value.b = logic_function(left->value.b, right->value.b, oper.logic);
            break;
        case NodeType::CONST_NODE:
            break;
        case NodeType::ATTR_NODE:
            switch (this->attrInfo.attrType) {
                case AttrType::INT:
                case AttrType::DATE:
                    this->value.i = *reinterpret_cast<int *>(data + this->attrInfo.attrOffset);
                    if (dataType == AttrType::FLOAT) {
                        this->value.f = static_cast<float>(this->value.i);
                    }
                    break;
                case AttrType::FLOAT:
                    this->value.f = *reinterpret_cast<float *>(data + this->attrInfo.attrOffset);
                    break;
                case AttrType::STRING:
                    this->value_s = std::string(data + this->attrInfo.attrOffset);
                    break;
                case AttrType::VARCHAR:
                case AttrType::NO_ATTR:
                    break;
                case AttrType::BOOL:
                    break;
            }
            break;
    }
}

void Expr::init_type() {
    if (left != nullptr) {
        left->init_type();
    }
    if (right != nullptr) {
        right->init_type();
    }
    switch (this->nodeType) {
        case NodeType::ARITH_NODE:
            switch (this->oper.arith) {
                case ArithOp::ADD_OP:
                case ArithOp::SUB_OP:
                case ArithOp::MUL_OP:
                case ArithOp::DIV_OP:
                    if ((left != nullptr and left->dataType == AttrType::FLOAT) or
                        (right != nullptr and right->dataType == AttrType::FLOAT)) {
                        dataType = AttrType::FLOAT;
                        if (left != nullptr) {
                            left->converToFloat();
                        }
                        if (right != nullptr) {
                            right->converToFloat();
                        }
                    } else {
                        dataType = AttrType::INT;
                    }
                    break;
                case ArithOp::MINUS_OP:
                    dataType = left->dataType;
                    break;
                case ArithOp::NO_OP:
                    dataType = AttrType::NO_ATTR;
                    break;
            }
            break;
        case NodeType::LOGIC_NODE:
            break;
        case NodeType::ATTR_NODE:
            break;
        case NodeType::COMP_NODE:
            dataType = AttrType::BOOL;
            break;
        case NodeType::CONST_NODE:
            dataType = oper.attr;
            break;
    }
}

void Expr::converToFloat() {
    if (left != nullptr) {
        left->init_type();
    }
    if (right != nullptr) {
        right->init_type();
    }
    switch (this->nodeType) {
        case NodeType::ARITH_NODE:
            dataType = AttrType::FLOAT;
            break;
        case NodeType::LOGIC_NODE:
            break;
        case NodeType::ATTR_NODE:
            dataType = AttrType::FLOAT;
            break;
        case NodeType::COMP_NODE:
            break;
        case NodeType::CONST_NODE:
            switch (oper.attr) {
                case AttrType::INT:
                case AttrType::DATE:
                    dataType = AttrType::FLOAT;
                    value.f = static_cast<float>(value.i);
                    break;
                case AttrType::FLOAT:
                    break;
                case AttrType::STRING:
                    break;
                case AttrType::VARCHAR:
                    break;
                case AttrType::BOOL:
                    break;
                case AttrType::NO_ATTR:
                    break;
            }
            break;
    }
}

