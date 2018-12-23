//
// Created by Zhengxiao Du on 2018-12-23.
//

#ifndef MYDB_FUNCTEMPLATE_H
#define MYDB_FUNCTEMPLATE_H

#include "../Constants.h"

template<typename T>
bool comp_function(const T &a, const T &b, CompOp compOp);

bool comp_function(const std::string &a, const std::string &b, CompOp compOp);

template<typename T>
T arith_function(T a, T b, ArithOp arithOp);

bool logic_function(bool a, bool b, LogicOp logicOp);

#endif //MYDB_FUNCTEMPLATE_H
