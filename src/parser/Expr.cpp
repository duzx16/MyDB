//
// Created by Zhengxiao Du on 2018-12-09.
//

#include "Expr.h"
#include "Tree.h"
#include "../utils/FuncTemplate.h"
#include <cstring>
#include <iostream>

std::string AttrTypeStr[] = {
        "INT", "FLOAT", "STRING", "DATE", "VARCHAR", "BOOL", "NO_ATTR"
};

std::string CompOpStr[] = {
        "=", "<", ">", "<=", ">=", "!=", "IS", "LIKE", "NO_OP"
};
std::string ArithOpStr[] = {
        "+", "-", "*", "/", "-", "NO"
};
std::string LogicOpStr[] = {
        "&&", "||", "!", "NO"
};

inline bool compatible(AttrType type1, AttrType type2) {
    return (type1 == AttrType::INT and type2 == AttrType::FLOAT) or
           (type1 == AttrType::FLOAT and type2 == AttrType::INT);
}

inline bool is_arithmetic(AttrType type) {
    return (type == AttrType::INT or type == AttrType::FLOAT);
}

Expr::Expr(int i) {
    is_null = false;
    calculated = true;
    value.i = i;
    nodeType = NodeType::CONST_NODE;
    oper.constType = AttrType::INT;
}

Expr::Expr(float f) {
    is_null = false;
    calculated = true;
    value.f = f;
    nodeType = NodeType::CONST_NODE;
    oper.constType = AttrType::FLOAT;
}

Expr::Expr(const char *s) {
    is_null = false;
    calculated = true;
    value_s = std::string(s, 1, strlen(s) - 2);;
    nodeType = NodeType::CONST_NODE;
    oper.constType = AttrType::STRING;
}


Expr::Expr(bool b) {
    is_null = true;
    calculated = true;
    value.b = b;
    oper.constType = AttrType::BOOL;
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

Expr::Expr(const AttributeNode *attributeNode) {
    is_null = false;
    this->attribute = attributeNode;
    this->nodeType = NodeType::ATTR_NODE;
    this->attrInfo.attrName = attributeNode->attribute;
    this->attrInfo.tableName = attributeNode->table;
}

void Expr::calculate(const char *data, const std::string &relationName) {
    if (calculated) {
        return;
    }
    if (left != nullptr) {
        left->calculate(data, relationName);
    }
    if (right != nullptr) {
        right->calculate(data, relationName);
    }
    switch (this->nodeType) {
        case NodeType::ARITH_NODE:
            if ((left != nullptr and !left->calculated) or (right != nullptr and !right->calculated)) {
                return;
            }
            calculated = true;
            if (left->is_null or (right != nullptr and right->is_null)) {
                is_null = true;
            } else {
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
            }
            break;
        case NodeType::COMP_NODE:
            if ((left != nullptr and !left->calculated) or (right != nullptr and !right->calculated)) {
                return;
            }
            calculated = true;
            if (oper.comp == CompOp::IS_OP) {
                value.b = not(left->is_null ^ right->is_null);
            } else {
                if (left->is_null or (right != nullptr and right->is_null)) {
                    is_null = true;
                } else {
                    switch (left->dataType) {
                        case AttrType::INT:
                        case AttrType::DATE:
                            value.b = comp_function(left->value.i, right->value.i, oper.comp);
                            break;
                        case AttrType::FLOAT:
                            value.b = comp_function(left->value.f, right->value.f, oper.comp);
                            break;
                        case AttrType::STRING:
                            value.b = comp_function(left->value_s, right->value_s, oper.comp);
                            break;
                        case AttrType::VARCHAR:
                        case AttrType::NO_ATTR:
                            break;
                        case AttrType::BOOL:
                            break;
                    }
                }
            }

            break;
        case NodeType::LOGIC_NODE:
            if ((left == nullptr or left->calculated) and (right == nullptr or right->calculated)) {
                calculated = true;
                if (right == nullptr) {
                    if (left->is_null) {
                        this->is_null = true;
                    } else {
                        this->value.b = logic_function(left->value.b, left->value.b, oper.logic);
                    }
                } else {
                    if (left->is_null or right->is_null) {
                        this->is_null = true;
                    } else {
                        this->value.b = logic_function(left->value.b, right->value.b, oper.logic);
                    }
                }
            } else {
                switch (oper.logic) {
                    case LogicOp::AND_OP:
                        if ((left->calculated and !left->value.b) or (right->calculated and !right->value.b)) {
                            calculated = true;
                            value.b = false;
                        }
                        break;
                    case LogicOp::OR_OP:
                        if ((left->calculated and left->value.b) or (right->calculated and right->value.b)) {
                            calculated = true;
                            value.b = true;
                        }
                        break;
                    case LogicOp::NOT_OP:
                    case LogicOp::NO_OP:
                        break;
                }
            }
            break;
        case NodeType::CONST_NODE:
            break;
        case NodeType::ATTR_NODE:
            if (relationName.empty() || relationName == attrInfo.tableName) {
                calculated = true;
                if (data[this->attrInfo.attrOffset - 1] == 0) {
                    is_null = true;
                } else {
                    switch (this->attrInfo.attrType) {
                        case AttrType::INT:
                        case AttrType::DATE:
                            this->value.i = *reinterpret_cast<const int *>(data + this->attrInfo.attrOffset);
                            if (dataType == AttrType::FLOAT) {
                                this->value.f = static_cast<float>(this->value.i);
                            }
                            break;
                        case AttrType::FLOAT:
                            this->value.f = *reinterpret_cast<const float *>(data + this->attrInfo.attrOffset);
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
                }
            }
            break;
    }
}

void Expr::type_check() {
    postorder([](Expr *expr) -> void {
        expr->dataType = AttrType::NO_ATTR;
        std::string operation;
        switch (expr->nodeType) {
            case NodeType::ARITH_NODE:
                operation = ArithOpStr[static_cast<int>(expr->oper.arith)];
                switch (expr->oper.arith) {
                    case ArithOp::ADD_OP:
                    case ArithOp::SUB_OP:
                    case ArithOp::MUL_OP:
                    case ArithOp::DIV_OP:
                        if (is_arithmetic(expr->left->dataType) and is_arithmetic(expr->right->dataType)) {
                            if (expr->left->dataType == expr->right->dataType) {
                                expr->dataType = expr->left->dataType;
                            } else if (compatible(expr->left->dataType, expr->right->dataType)) {
                                expr->left->convert_to_float();
                                expr->right->convert_to_float();
                                expr->dataType = AttrType::FLOAT;
                            }
                        }
                        break;
                    case ArithOp::MINUS_OP:
                        if (expr->left->dataType == AttrType::INT or expr->left->dataType == AttrType::FLOAT)
                            expr->dataType = expr->left->dataType;
                        break;
                    case ArithOp::NO_OP:
                        break;
                }
                break;
            case NodeType::LOGIC_NODE:
                operation = LogicOpStr[static_cast<int>(expr->oper.logic)];
                switch (expr->oper.logic) {
                    case LogicOp::AND_OP:
                    case LogicOp::OR_OP:
                        if (expr->left->dataType == AttrType::BOOL and expr->right->dataType == AttrType::BOOL) {
                            expr->dataType = AttrType::BOOL;
                        }
                        break;
                    case LogicOp::NOT_OP:
                        if (expr->left->dataType == AttrType::BOOL) {
                            expr->dataType = AttrType::BOOL;
                        }
                        break;
                    case LogicOp::NO_OP:
                        break;
                }
                break;
            case NodeType::ATTR_NODE:
                expr->dataType = expr->attrInfo.attrType;
                break;
            case NodeType::COMP_NODE:
                operation = CompOpStr[static_cast<int>(expr->oper.comp)];
                switch (expr->oper.comp) {
                    case CompOp::EQ_OP:
                    case CompOp::LT_OP:
                    case CompOp::GT_OP:
                    case CompOp::LE_OP:
                    case CompOp::GE_OP:
                    case CompOp::NE_OP:
                        if (expr->left->dataType == expr->right->dataType) {
                            expr->dataType = AttrType::BOOL;
                        } else if (compatible(expr->left->dataType, expr->right->dataType)) {
                            expr->dataType = AttrType::BOOL;
                            expr->left->convert_to_float();
                            expr->right->convert_to_float();
                        }
                        break;
                    case CompOp::IS_OP:
                        expr->dataType = AttrType::BOOL;
                        break;
                    case CompOp::LIKE_OP:
                        if ((expr->left->dataType == AttrType::VARCHAR or expr->left->dataType == AttrType::STRING) and
                            (expr->right->dataType == AttrType::VARCHAR or expr->right->dataType == AttrType::STRING)) {
                            expr->dataType = AttrType::BOOL;
                        }
                        break;
                    case CompOp::NO_OP:
                        break;
                }
                break;
            case NodeType::CONST_NODE:
                expr->dataType = expr->oper.constType;
                break;
        }
        if (expr->dataType == AttrType::NO_ATTR) {
            if (expr->right == nullptr) {
                if (expr->left->dataType != AttrType::NO_ATTR) {
                    std::cerr << "Unsupported operation " << operation << " for type "
                              << AttrTypeStr[static_cast<int>(expr->left->dataType)] << "\n";
                }
            } else {
                if (expr->left->dataType != AttrType::NO_ATTR and expr->right->dataType != AttrType::NO_ATTR) {
                    std::cerr << "Unsupported operation " << operation << " for type "
                              << AttrTypeStr[static_cast<int>(expr->left->dataType)] << " and "
                              << AttrTypeStr[static_cast<int>(expr->right->dataType)] << "\n";
                }
            }
        }
    });
}

void Expr::convert_to_float() {
    postorder([](Expr *expr) -> void {
        switch (expr->nodeType) {
            case NodeType::ARITH_NODE:
                expr->dataType = AttrType::FLOAT;
                break;
            case NodeType::LOGIC_NODE:
                break;
            case NodeType::ATTR_NODE:
                expr->dataType = AttrType::FLOAT;
                break;
            case NodeType::COMP_NODE:
                break;
            case NodeType::CONST_NODE:
                switch (expr->oper.constType) {
                    case AttrType::INT:
                    case AttrType::DATE:
                        expr->dataType = AttrType::FLOAT;
                        expr->value.f = static_cast<float>(expr->value.i);
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
    }, [](Expr *expr) -> bool {
        return expr->dataType == AttrType::FLOAT;
    });
}

void Expr::postorder(std::function<void(Expr *)> callback, std::function<bool(Expr *)> stop_condition) {
    if (stop_condition != nullptr) {
        if (stop_condition(this)) {
            return;
        }
    }
    if (left != nullptr) {
        callback(left);
    }
    if (right != nullptr) {
        callback(right);
    }
    callback(this);
}

bool judge_calculated(Expr *expr) {
    if (expr->left != nullptr) {
        if (expr->right != nullptr) {
            return expr->left->calculated and expr->right->calculated;
        } else {
            return expr->left->calculated;
        }
    }
    return false;
}

void Expr::init_calculate(const std::string &tableName) {
    if (tableName.empty()) {
        postorder([](Expr *expr) -> void {
            switch (expr->nodeType) {
                case NodeType::ARITH_NODE:
                case NodeType::COMP_NODE:
                case NodeType::LOGIC_NODE:
                    expr->calculated = false;
                    break;
                case NodeType::CONST_NODE:
                    expr->calculated = true;
                    break;
                case NodeType::ATTR_NODE:
                    expr->calculated = false;
                    break;
            }
        });
    } else {
        postorder([&tableName, this](Expr *expr) -> void {
            switch (expr->nodeType) {
                case NodeType::ARITH_NODE:
                case NodeType::COMP_NODE:
                case NodeType::LOGIC_NODE:
                    expr->calculated = judge_calculated(this);
                    break;
                case NodeType::CONST_NODE:
                    expr->calculated = true;
                    break;
                case NodeType::ATTR_NODE:
                    if (expr->attrInfo.tableName == tableName) {
                        expr->calculated = false;
                    }
            }
        });
    }
}

bool Expr::operator<(const Expr &expr) {
    if (calculated and expr.calculated) {
        switch (dataType) {
            case AttrType::INT:
            case AttrType::DATE:
                return value.i < expr.value.i;
            case AttrType::FLOAT:
                return value.f < expr.value.f;
            case AttrType::STRING:
            case AttrType::VARCHAR:
                return value_s < expr.value_s;
            case AttrType::BOOL:
                return value.b < expr.value.b;
            default:
                return false;
        }
    } else {
        throw "Can't perform comparison because the value is uncertain\n";
    }
}

Expr &Expr::operator+=(const Expr &expr) {
    if (calculated and expr.calculated) {
        switch (dataType) {
            case AttrType::INT:
            case AttrType::DATE:
                value.i += expr.value.i;
                break;
            case AttrType::FLOAT:
                value.f += expr.value.f;
                break;
            case AttrType::STRING:
            case AttrType::VARCHAR:
            case AttrType::BOOL:
            case AttrType::NO_ATTR:
                // unsupported
                break;
        }
        return *this;
    } else {
        throw "Can't perform computation because the value is uncertain\n";
    }
}

void Expr::assign(const Expr &expr) {
    if (calculated and expr.calculated) {
        switch (dataType) {
            case AttrType::INT:
            case AttrType::DATE:
                value.i = expr.value.i;
                break;
            case AttrType::FLOAT:
                value.f = expr.value.f;
                break;
            case AttrType::STRING:
                value_s = expr.value_s;
                break;
            case AttrType::VARCHAR:
            case AttrType::BOOL:
            case AttrType::NO_ATTR:
                // unsupported
                break;
        }
    } else {
        throw "Can't perform computation because the value is uncertain\n";
    }
}

std::string Expr::to_string() const {
    switch (dataType) {
        case AttrType::INT:
            return std::to_string(value.i);
        case AttrType::FLOAT:
            return std::to_string(value.f);
        case AttrType::STRING:
        case AttrType::VARCHAR:
            return std::string(value_s);
        case AttrType::DATE:
            //todo implement this
            break;
        case AttrType::BOOL:
            break;
        case AttrType::NO_ATTR:
            break;
    }
    return std::string();
}

