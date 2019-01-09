//
// Created by Zhengxiao Du on 2018-12-23.
//

#ifndef MYDB_FUNCTEMPLATE_H
#define MYDB_FUNCTEMPLATE_H

#include "../Constants.h"

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

bool comp_function(const std::string &a, const std::string &b, CompOp compOp);

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

bool logic_function(bool a, bool b, LogicOp logicOp);
bool isComparison(CompOp op);

#endif //MYDB_FUNCTEMPLATE_H
