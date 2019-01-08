//
// Created by Zhengxiao Du on 2019-01-01.
//

#ifndef MYDB_AGGREGATION_H
#define MYDB_AGGREGATION_H

#include "../parser/Expr.h"
#include <map>

class Aggregation {
public:
    Aggregation(const BindAttribute &attrInfo, AggregationType type, bool isGroup);

    void accumulate(const std::string &group, const Expr &value);

    ~Aggregation();

    Expr *expr = nullptr;
    std::map<std::string, Expr *> groupData;
    std::map<std::string, int> groupCount;

private:
    AggregationType type;
    BindAttribute attrInfo;
    bool is_group;
};


#endif //MYDB_AGGREGATION_H
