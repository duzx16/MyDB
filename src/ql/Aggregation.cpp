//
// Created by Zhengxiao Du on 2019-01-01.
//

#include "Aggregation.h"
#include <climits>

Expr *init_statistics(AttrType attrType, AggregationType aggregation) {
    Expr *result = nullptr;
    switch (aggregation) {
        case AggregationType::T_AVG:
        case AggregationType::T_SUM:
            switch (attrType) {
                case AttrType::INT:
                    result = new Expr(0);
                    break;
                case AttrType::FLOAT:
                    result = new Expr(0.0f);
                    break;
                default:
                    break;
                    // this will never happen
            }
            break;
        case AggregationType::T_MIN:
            switch (attrType) {
                case AttrType::INT:
                    result = new Expr(INT_MAX);
                    break;
                case AttrType::FLOAT:
                    result = new Expr(1e38f);
                    break;
                default:
                    break;
            }
            break;
        case AggregationType::T_MAX:
            switch (attrType) {
                case AttrType::INT:
                    result = new Expr(INT_MIN);
                    break;
                case AttrType::FLOAT:
                    result = new Expr(-1e38f);
                    break;
                default:
                    break;
            }
            break;
        case AggregationType::T_NONE:
            break;
    }
    if (result != nullptr) {
        result->type_check();
    }
    return result;
}

Aggregation::Aggregation(const BindAttribute &attrInfo, AggregationType type, bool isGroup) {
    is_group = isGroup;
    this->type = type;
    this->attrInfo = attrInfo;
    if (this->type != AggregationType::T_NONE) {
        if (this->attrInfo.attrType != AttrType::INT and this->attrInfo.attrType != AttrType::FLOAT) {
            throw "The type of column " + attrInfo.attrName + " can't be aggregated\n";
        }
    }
    expr = nullptr;
    if (not isGroup) {
        expr = init_statistics(attrInfo.attrType, type);
    }
    if (expr != nullptr) {
        expr->type_check();
    }
}

Aggregation::~Aggregation() {
    delete expr;
}

void Aggregation::accumulate(const std::string &group, const Expr &value) {
    if (not value.is_null) {
        Expr *result;
        if (not is_group) {
            result = expr;
        } else {
            if (groupData.find(group) == groupData.end()) {
                result = init_statistics(attrInfo.attrType, type);
                groupData[group] = result;
                groupCount[group] = 1;
            } else {
                result = groupData[group];
                groupCount[group] += 1;
            }
        }
        switch (type) {
            case AggregationType::T_AVG:
            case AggregationType::T_SUM:
                *result += value;
                break;
            case AggregationType::T_MIN:
                if (value < *result) {
                    (*result).assign(value);
                }
                break;
            case AggregationType::T_MAX:
                if (*result < value) {
                    (*result).assign(value);
                }
                break;
            case AggregationType::T_NONE:
                break;
        }
    }

}
