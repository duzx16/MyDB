#include <memory>
#include <map>
#include <utility>

//
// Created by Zhengxiao Du on 2018-12-09.
//

#include "QueryManager.h"
#include "../rm/RM_FileHandle.h"
#include "../rm/RecordManager.h"
#include "../rm/RM_FileScan.h"
#include "../parser/Expr.h"

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

int
QL_Manager::exeSelect(AttributeList *attributes, IdentList *relations, Expr *whereClause,
                      const std::string &grouAttrName) {
    // if groupAttrName not empty, there must be statistics and only one non-statistics attribute
    // statistics can't appear along with normal attribute
    // aggregate can only appear for numeric column
    std::vector<std::unique_ptr<Table>> tables;
    int rc = 0;
    try {
        for (const auto &ident : relations->idents) {
            auto *table = new Table(ident);
            tables.push_back(std::unique_ptr<Table>(table));
        }
    }
    catch (const std::string &error) {
        cerr << error;
        return QL_TABLE_FAIL;
    }
    std::vector<Expr> attributeExprs;
    std::vector<int> taleIndexs;
    std::vector<void *> statistics;
    bool isStatistic = false;
    bool showGroup = false;
    std::map<std::string, int> group_count;
    int total_count = 0;
    AttributeNode groupAttribute{grouAttrName.c_str()};
    Expr groupAttrExpr{&groupAttribute};
    int groupAttrTableIndex;
    try {
        if (attributes != nullptr) {
            if (!grouAttrName.empty()) {
                bindAttribute(&groupAttrExpr, tables);
                groupAttrExpr.type_check();
                for (int i = 0; i < tables.size(); ++i) {
                    if (tables[i]->tableName == groupAttrExpr.attrInfo.tableName) {
                        groupAttrTableIndex = i;
                        break;
                    }
                }
            }
            for (auto &attribute: attributes->attributes) {
                attributeExprs.emplace_back(attribute);
                bindAttribute(&attributeExprs.back(), tables);
                attributeExprs.back().type_check();
                for (int i = 0; i < tables.size(); ++i) {
                    if (tables[i]->tableName == attributeExprs.back().attrInfo.tableName) {
                        taleIndexs.push_back(i);
                        if (attribute->aggregationType != AggregationType::T_NONE) {
                            if (grouAttrName.empty()) {
                                Expr *result = init_statistics(attributeExprs.back().dataType,
                                                               attribute->aggregationType);
                                statistics.push_back(result);
                            } else {
                                statistics.push_back(new std::map<std::string, Expr *>());
                            }
                            isStatistic = true;
                        } else {
                            showGroup = true;
                            if (grouAttrName.empty()) {
                                statistics.push_back(nullptr);
                            } else {
                                statistics.push_back(new std::map<std::string, Expr *>());
                            }
                        }
                        break;
                    }
                }
            }
        }
        bindAttribute(whereClause, tables);
    } catch (AttrBindException &exception) {
        printException(exception);
        return QL_TABLE_FAIL;
    }
    iterateRecords(tables.begin(), tables.end(), whereClause,
                   [this, &tables, attributes, &taleIndexs, isStatistic, &attributeExprs, &grouAttrName, &statistics, &group_count, &total_count, &groupAttrExpr, groupAttrTableIndex](
                           const RM_Record &record) -> void {
                       total_count += 1;
                       if (attributes == nullptr) {
                           for (int i = 0; i < tables.size(); ++i) {
                               if (i == tables.size() - 1) {
                                   cout << tables.back()->printData(record.getData()) << "\n";
                               } else {
                                   cout << tables[i]->printData(recordCaches[i]->getData()) << "\t";
                               }
                           }
                       } else {
                           for (int i = 0; i < attributeExprs.size(); ++i) {
                               const char *data;
                               int tableIndex = taleIndexs[i];
                               if (tableIndex == tables.size() - 1) {
                                   data = record.getData();
                               } else {
                                   data = recordCaches[tableIndex]->getData();
                               }
                               Expr &attributeExpr = attributeExprs[i];
                               attributeExpr.calculate(data);
                               if (isStatistic) {
                                   if (statistics[i] != nullptr) {
                                       Expr *result;
                                       if (grouAttrName.empty()) {
                                           result = reinterpret_cast<Expr *>(statistics[i]);
                                       } else {
                                           const char *group_data;
                                           if (groupAttrTableIndex == tables.size() - 1) {
                                               group_data = record.getData();
                                           } else {
                                               group_data = recordCaches[groupAttrTableIndex]->getData();
                                           }
                                           groupAttrExpr.calculate(group_data);
                                           auto &attr_map = *reinterpret_cast<std::map<std::string, Expr *> *>(statistics[i]);
                                           std::string attribute_str = groupAttrExpr.to_string();
                                           if (attr_map.find(attribute_str) == attr_map.end()) {
                                               result = init_statistics(attributeExpr.dataType,
                                                                        attributeExpr.attribute->aggregationType);
                                               attr_map[attribute_str] = result;
                                               group_count[attribute_str] = 0;
                                           } else {
                                               result = attr_map[attribute_str];
                                               group_count[attribute_str] += 1;
                                           }
                                           groupAttrExpr.init_calculate();
                                       }
                                       switch (attributeExpr.attribute->aggregationType) {
                                           case AggregationType::T_AVG:
                                           case AggregationType::T_SUM:
                                               *result += attributeExpr;
                                               break;
                                           case AggregationType::T_MIN:
                                               if (attributeExpr < *result) {
                                                   (*result).assign(attributeExpr);
                                               }
                                               break;
                                           case AggregationType::T_MAX:
                                               if (*result < attributeExpr) {
                                                   (*result).assign(attributeExpr);
                                               }
                                               break;
                                           case AggregationType::T_NONE:
                                               break;
                                       }
                                   } else {
                                       if (!grouAttrName.empty()) {
                                           auto &attr_map = *reinterpret_cast<std::map<std::string, Expr *> *>(statistics[i]);
                                           std::string attribute_str = attributeExpr.to_string();
                                           attr_map[attribute_str] = nullptr;
                                       }
                                   }
                               } else {
                                   if (i == attributes->attributes.size() - 1) {
                                       cout << attributeExpr.to_string() << "\n";
                                   } else {
                                       cout << attributeExpr.to_string() << "\t";
                                   }
                               }
                               attributeExpr.init_calculate();
                           }
                       }


                   });

    if (isStatistic) {
        if (grouAttrName.empty()) {
            for (int i = 0; i < attributes->attributes.size(); ++i) {
                Expr *result = reinterpret_cast<Expr *>(statistics[i]);
                if (attributes->attributes[i]->aggregationType == AggregationType::T_AVG) {
                    if (result->dataType != AttrType::FLOAT) {
                        result->dataType = AttrType::FLOAT;
                        result->value.f = result->value.i;
                    }
                    result->value.f /= total_count;

                }
                cout << result->to_string() << "\t";
            }
            cout << "\n";
        } else {
            auto &attr_map = *reinterpret_cast<std::map<std::string, Expr *> *>(statistics[0]);
            for (const auto &entry: attr_map) {
                for (int i = 0; i < attributes->attributes.size(); ++i) {
                    const AttributeNode &attribute = *attributes->attributes[i];
                    if (attribute.aggregationType == AggregationType::T_NONE) {
                        cout << entry.first << "\t";
                    } else {
                        Expr *result = (*reinterpret_cast<std::map<std::string, Expr *> *>(statistics[i]))[entry.first];
                        if (attribute.aggregationType == AggregationType::T_AVG) {
                            if (result->dataType != AttrType::FLOAT) {
                                result->dataType = AttrType::FLOAT;
                                result->value.f = result->value.i;
                            }
                            result->value.f /= group_count[entry.first];
                        }
                        cout
                                << result->to_string()
                                << "\t";
                    }
                }
                cout << "\n";
            }
        }
    }
    return 0;
}

int QL_Manager::exeInsert(std::string relationName, IdentList *columnList, ConstValueLists *insertValueTree) {
    std::vector<std::unique_ptr<Table>> tables;
    try {
        tables.push_back(std::make_unique<Table>(relationName));
    }
    catch (const std::string &error) {
        cerr << error;
        return QL_TABLE_FAIL;
    }
    try {
        for (const auto &values: insertValueTree->values) {
            tables[0]->insertData(columnList, values);
        }
    }
    catch (const std::string &strerror) {
        cerr << strerror;
    }
    return 0;
}

int QL_Manager::exeUpdate(std::string relationName, SetClauseList *setClauses, Expr *whereClause) {
    std::vector<std::unique_ptr<Table>> tables;
    try {
        tables.push_back(std::make_unique<Table>(relationName));
    }
    catch (const std::string &error) {
        cerr << error;
        return QL_TABLE_FAIL;
    }
    try {
        bindAttribute(whereClause, tables);
        for (auto &it: setClauses->clauses) {
            bindAttribute(it.second, tables);
        }
    } catch (AttrBindException &exception) {
        printException(exception);
        return QL_TABLE_FAIL;
    }
    std::vector<int> attributeIndexs;
    for (const auto &it: setClauses->clauses) {
        int index = tables[0]->getColumnIndex(it.first->attribute);
        if (index < 0) {
            fprintf(stderr, "The column %s doesn't exist.\n", it.first->attribute.c_str());
            return -1;
        }
        attributeIndexs.push_back(index);
    }
    iterateRecords(tables.begin(), tables.end(), whereClause,
                   [setClauses, &tables, &attributeIndexs](const RM_Record &record) -> void {
                       for (auto &it: setClauses->clauses) {
                           it.second->init_calculate();
                           it.second->calculate(record.getData());
                       }
                       tables[0]->updateData(record, attributeIndexs, setClauses);
                   });
    return 0;
}

int QL_Manager::exeDelete(std::string relationName, Expr *whereClause) {
    std::vector<std::unique_ptr<Table>> tables;
    try {
        tables.push_back(std::make_unique<Table>(relationName));
    }
    catch (const std::string &error) {
        cerr << error;
        return QL_TABLE_FAIL;
    }
    try {
        bindAttribute(whereClause, tables);
    } catch (AttrBindException &exception) {
        printException(exception);
        return QL_TABLE_FAIL;
    }
    std::vector<RID> toBeDeleted;
    iterateRecords(tables.begin(), tables.end(), whereClause,
                   [&toBeDeleted](const RM_Record &record) -> void {
                       toBeDeleted.push_back(record.getRID());
                   });
    RM_FileHandle &fileHandle = tables[0]->getFileHandler();
    for (auto &it: toBeDeleted) {
        int rc = tables[0]->deleteData(it);
        if (rc != 0) {
            cerr << "Delete error\n";
            return -1;
        }
    }
    return 0;
}

void QL_Manager::printException(const AttrBindException &exception) {
    switch (exception.type) {
        case EXPR_NO_SUCH_TABLE:
            fprintf(stderr, "Can't find the table %s\n", exception.table.c_str());
            break;
        case EXPR_NO_SUCH_ATTRIBUTE:
            fprintf(stderr, "Can't find the attribute %s in table %s\n", exception.attribute.c_str(),
                    exception.table.c_str());
            break;
        case EXPR_AMBIGUOUS:
            fprintf(stderr, "Reference to the attribute %s is ambiguous\n", exception.attribute.c_str());
            break;
        default:
            fprintf(stderr, "Unknown error\n");
            break;
    }
}

int QL_Manager::iterateRecords(Table &table, Expr *condition, QL_Manager::CallbackFunc callback) {
    int rc;
    RM_FileScan fileScan;
    fileScan.openScan(table.getFileHandler(), condition, table.tableName);
    while (true) {
        RM_Record record;
        rc = fileScan.getNextRec(record);
        if (rc) {
            break;
        }
        callback(record);
    }
    return 0;
}

void QL_Manager::bindAttribute(Expr *expr, const std::vector<std::unique_ptr<Table>> &tables) {
    expr->postorder([&tables](Expr *expr1) {
        if (expr1->nodeType == NodeType::ATTR_NODE) {
            if (!expr1->attribute->table.empty()) {
                for (const auto &table: tables) {
                    if (table->tableName == expr1->attribute->table) {
                        int offset = table->getOffset(expr1->attribute->attribute);
                        if (offset > 0) {
                            ColumnNode *column = table->getColumn(expr1->attribute->attribute);
                            expr1->attrInfo.attrType = column->type;
                            expr1->attrInfo.attrOffset = offset;
                            expr1->attrInfo.attrLength = column->size;
                            expr1->attrInfo.tableName = table->tableName;
                            expr1->dataType = column->type;
                            return;
                        } else {
                            throw AttrBindException{"", expr1->attribute->attribute, EXPR_NO_SUCH_ATTRIBUTE};
                        }
                    }
                }
                throw AttrBindException{expr1->attribute->table, "", EXPR_NO_SUCH_TABLE};
            } else {
                expr1->attrInfo.attrOffset = -1;
                for (const auto &table: tables) {
                    if (not expr1->attrInfo.tableName.empty()) {
                        throw AttrBindException{"", expr1->attribute->attribute, EXPR_AMBIGUOUS};
                    } else {
                        int offset = table->getOffset(expr1->attribute->attribute);
                        if (offset > 0) {
                            ColumnNode *column = table->getColumn(expr1->attribute->attribute);
                            expr1->attrInfo.attrType = column->type;
                            expr1->attrInfo.attrOffset = offset;
                            expr1->attrInfo.attrLength = column->size;
                            expr1->attrInfo.tableName = table->tableName;
                            expr1->dataType = column->type;
                        }
                    }
                }
                if (expr1->attrInfo.tableName.empty()) {
                    throw AttrBindException{"", expr1->attribute->attribute, EXPR_NO_SUCH_ATTRIBUTE};
                }
            }
        }
    });
}

QL_Manager &QL_Manager::getInstance() {
    static QL_Manager instance;
    return instance;
}

int QL_Manager::iterateRecords(tableListIter begin, tableListIter end, Expr *condition,
                               QL_Manager::CallbackFunc callback) {
    if ((end - begin) == 1) {
        return iterateRecords(**begin, condition, std::move(callback));
    }
    //todo complete multiple tables caches
    return 0;
}
