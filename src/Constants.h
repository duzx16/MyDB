//
// Created by Zhengxiao Du on 2018/10/28.
//

#ifndef MYDB_CONSTANTS_H
#define MYDB_CONSTANTS_H

/*
 * 一个页面中的字节数
 */
#define PAGE_SIZE 8192
typedef unsigned int* BufType;
enum class AttrType{INT, FLOAT, STRING, NO_ATTR};
enum class CompOp{EQ_OP, LT_OP, GT_OP, LE_OP, GE_OP, NE_OP, NO_OP};

#endif //MYDB_CONSTANTS_H
