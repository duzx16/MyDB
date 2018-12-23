//
// Created by Zhengxiao Du on 2018/10/28.
//

#ifndef MYDB_CONSTANTS_H
#define MYDB_CONSTANTS_H

#include <string>

/*
 * 一个页面中的字节数
 */
typedef unsigned int *BufType;
enum class AttrType {
    INT, FLOAT, STRING, DATE, VARCHAR, BOOL, NO_ATTR
};
enum class CompOp {
    EQ_OP, LT_OP, GT_OP, LE_OP, GE_OP, NE_OP, IS_OP, LIKE_OP, NO_OP
};
enum class ArithOp {
    ADD_OP, SUB_OP, MUL_OP, DIV_OP, MINUS_OP, NO_OP
};
enum class LogicOp {
    AND_OP, OR_OP, NO_OP
};

enum class AggregationType {
    T_NONE = 0,
    T_AVG,
    T_SUM,
    T_MIN,
    T_MAX
};

enum class ConstraintType {
    PRIMARY_CONSTRAINT,
    FOREIGN_CONSTRAINT,
    CHECK_CONSTRAINT
};

#define MAX_NAME 42
#define COLUMN_FLAG_NOTNULL 0x1

struct AttrInfo {
    std::string attrName;
    std::string tableName;
    int attrLength{};
    AttrType attrType = AttrType::NO_ATTR;
    bool notNull{};
    bool withIndex;
    int attrOffset{};
};

//
// redbase.h
//   global declarations
//
#ifndef REDBASE_H
#define REDBASE_H

//
// Return codes
//
typedef int RC;

#define OK_RC         0    // OK_RC return code is guaranteed to always be 0

#define START_PF_ERR  (-1)
#define END_PF_ERR    (-100)
#define START_RM_ERR  (-101)
#define END_RM_ERR    (-200)
#define START_IX_ERR  (-201)
#define END_IX_ERR    (-300)
#define START_SM_ERR  (-301)
#define END_SM_ERR    (-400)
#define START_QL_ERR  (-401)
#define END_QL_ERR    (-500)

#define START_PF_WARN  1
#define END_PF_WARN    100
#define START_RM_WARN  101
#define END_RM_WARN    200
#define START_IX_WARN  201
#define END_IX_WARN    300
#define START_SM_WARN  301
#define END_SM_WARN    400
#define START_QL_WARN  401
#define END_QL_WARN    500

// ALL_PAGES is defined and used by the ForcePages method defined in RM
// and PF layers
const int ALL_PAGES = -1;

//
// TRUE, FALSE and BOOLEAN
//
#ifndef BOOLEAN
typedef char Boolean;
#endif

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE 1
#endif

#ifndef NULL
#define NULL 0
#endif

#endif


#endif //MYDB_CONSTANTS_H
