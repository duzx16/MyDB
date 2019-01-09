//
// Created by Zhengxiao Du on 2018-12-23.
//

#include "FuncTemplate.h"
#include <regex>

bool comp_function(const std::string &a, const std::string &b, CompOp compOp) {
    if (compOp == CompOp::LIKE_OP) {
        std::string regexStr;
        char status = 'A';
        for (char i : b) {
            if (status == 'A') {
                // common status
                if (i == '\\') {
                    status = 'B';
                } else if (i == '[') {
                    regexStr += "[";
                    status = 'C';
                } else if (i == '%') {
                    regexStr += ".*";
                } else if (i == '_') {
                    regexStr += ".";
                } else {
                    regexStr += i;
                }
            } else if (status == 'B') {
                // after '\'
                if (i == '%' || i == '_' || i == '!') {
                    regexStr += i;
                } else {
                    regexStr += "\\";
                    regexStr += i;
                }
                status = 'A';
            } else {
                // after '[' inside []
                if (i == '!') {
                    regexStr += "^";
                } else {
                    regexStr += i;
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
            default:
                return false;
        }
    }
    return false;
}

bool logic_function(bool a, bool b, LogicOp logicOp) {
    switch (logicOp) {
        case LogicOp::AND_OP:
            return a and b;
        case LogicOp::OR_OP:
            return a or b;
        case LogicOp::NOT_OP:
            return not a;
        case LogicOp::NO_OP:
            break;
    }
    return false;
}

bool isComparison(CompOp op) {
    switch (op) {
        case CompOp::EQ_OP:
        case CompOp::GE_OP:
        case CompOp::GT_OP:
        case CompOp::LE_OP:
        case CompOp::LT_OP:
        case CompOp::NE_OP:
            return true;
        case CompOp::NO_OP:
        case CompOp::LIKE_OP:
        case CompOp::IS_OP:
        case CompOp::ISNOT_OP:
            return false;
    }
    return false;
}

