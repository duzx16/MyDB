#include <utility>

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

#include <set>

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

int QL_Manager::openTables(const std::vector<std::string> &tableNames, std::vector<std::unique_ptr<Table>> &tables) {
    try {
        for (const auto &ident : tableNames) {
            tables.push_back(make_unique<Table>(ident));
        }
    }
    catch (const std::string &error) {
        cerr << error;
        return QL_TABLE_FAIL;
    }
    return 0;
}

int QL_Manager::whereBindCheck(Expr *whereClause, std::vector<std::unique_ptr<Table>> &tables) {
    try {
        bindAttribute(whereClause, tables);
        whereClause->type_check();
        if (whereClause->dataType != AttrType::BOOL) {
            if (whereClause->dataType != AttrType::NO_ATTR) {
                cerr << "where clause type error\n";
            }
            return QL_TYPE_CHECK;
        }
        return 0;
    } catch (AttrBindException &exception) {
        printException(exception);
        return QL_TABLE_FAIL;
    }
}

std::set<int> collectTable(Expr &expr, const std::set<int> &before) {
    //return the tables in expr
    std::set<int> tableNums;
    expr.postorder([&tableNums, &before](Expr *expr) -> void {
        if (expr->nodeType == NodeType::ATTR_NODE) {
            if (before.find(expr->tableIndex) != before.end()) {
                tableNums.insert(expr->tableIndex);
            }
        }
    });
    return tableNums;
}

void optimizeIteration(std::vector<std::unique_ptr<Table>> &tables, Expr *condition, std::vector<int> &iterationRank,
                       std::list<Expr *> &indexExprs, std::set<int> &before) {
    std::set<int> bestRight;
    Expr *indexAim = nullptr;
    Expr *current = condition;
    while (current != nullptr) {
        if (current->calculated or current->nodeType != NodeType::LOGIC_NODE or
            current->oper.logic != LogicOp::AND_OP) {
            break;
        }
        Expr *aim = current->right;
        if (not aim->calculated and aim->nodeType == NodeType::COMP_NODE) {
            // the left of comparison must be attribute
            // the left has not been iterated
            if (before.find(aim->left->tableIndex) == before.end()) {
                Table &table = *tables[aim->left->tableIndex];
                // the left is indexed
                if (table.getIndexAvailable(aim->left->columnIndex)) {
                    std::set<int> rightTables = collectTable(*aim->right, before);
                    // the left table not appear in right
                    if (rightTables.find(aim->left->tableIndex) == rightTables.end() &&
                        rightTables.size() < bestRight.size()) {
                        indexAim = aim;
                        bestRight = rightTables;
                    }
                }
            }
        }
        current = current->left;
    }
    if (indexAim != nullptr) {
        for (auto &index: bestRight) {
            iterationRank.push_back(index);
            before.insert(index);
        }
        iterationRank.push_back(indexAim->left->tableIndex);
        before.insert(indexAim->left->tableIndex);
        indexExprs.push_back(indexAim);
        optimizeIteration(tables, condition, iterationRank, indexExprs, before);
    } else {
        for (int i = 0; i < tables.size(); ++i) {
            if (before.find(i) == before.end()) {
                iterationRank.push_back(i);
            }
        }
    }
}

int
QL_Manager::exeSelect(AttributeList *attributes, IdentList *relations, Expr *whereClause,
                      const std::string &grouAttrName) {
    // if groupAttrName not empty, there must be statistics and only one non-statistics attribute
    // statistics can't appear along with normal attribute
    // aggregate can only appear for numeric column
    std::vector<std::unique_ptr<Table>> tables;
    int rc = 0;
    rc = openTables(relations->idents, tables);
    if (rc != 0) {
        return rc;
    }
    rc = whereBindCheck(whereClause, tables);
    if (rc != 0) {
        return rc;
    }
    // optimize the iteration
    std::vector<int> iterationRank;
    std::list<Expr *> indexExprs;
    std::set<int> before;
    optimizeIteration(tables, whereClause, iterationRank, indexExprs, before);
    std::vector<std::unique_ptr<Table>> newTables;
    newTables.reserve(iterationRank.size());
    for (int i : iterationRank) {
        newTables.push_back(std::move(tables[i]));
    }
    whereClause->postorder([&newTables](Expr *expr) -> void {
        if (expr->nodeType == NodeType::ATTR_NODE) {
            for (int i = 0; i < newTables.size(); ++i) {
                if (expr->attrInfo.tableName == newTables[i]->tableName) {
                    expr->tableIndex = i;
                    break;
                }
            }
        }
    });
    // bind the attributes
    std::vector<Expr> attributeExprs;
    std::vector<void *> statistics;
    bool isStatistic = false;
    std::map<std::string, int> group_count;
    int total_count = 0;
    AttributeNode groupAttribute{grouAttrName.c_str()};
    Expr groupAttrExpr{&groupAttribute};
    try {
        if (attributes != nullptr) {
            if (!grouAttrName.empty()) {
                bindAttribute(&groupAttrExpr, newTables);
                groupAttrExpr.type_check();
            }
            for (auto &attribute: attributes->attributes) {
                attributeExprs.emplace_back(attribute);
                bindAttribute(&attributeExprs.back(), newTables);
                attributeExprs.back().type_check();
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
                    if (grouAttrName.empty()) {
                        statistics.push_back(nullptr);
                    } else {
                        statistics.push_back(new std::map<std::string, Expr *>());
                    }
                }
            }
        } else {
            for (int j = 0; j < newTables.size(); ++j) {
                const auto &table = newTables[j];
                for (int i = 0; i < table->getAttrCount(); ++i) {
                    attributeExprs.emplace_back(table->getAttrInfo(i));
                    attributeExprs.back().tableIndex = j;
                }
            }
        }

    } catch (AttrBindException &exception) {
        printException(exception);
        return QL_TABLE_FAIL;
    }
    // begin iterate
    iterateTables(newTables, 0, whereClause,
                  [this, isStatistic, &attributeExprs, &grouAttrName, &statistics, &group_count, &total_count, &groupAttrExpr](
                          const std::vector<RM_Record> &caches) -> void {
                      total_count += 1;
                      for (int i = 0; i < attributeExprs.size(); ++i) {
                          const char *data;
                          Expr &attributeExpr = attributeExprs[i];
                          int tableIndex = attributeExpr.tableIndex;
                          data = caches[tableIndex].getData();
                          attributeExpr.calculate(data);
                          if (isStatistic) {
                              if (statistics[i] != nullptr) {
                                  Expr *result;
                                  if (grouAttrName.empty()) {
                                      result = reinterpret_cast<Expr *>(statistics[i]);
                                  } else {
                                      const char *group_data;
                                      int groupAttrTableIndex = groupAttrExpr.tableIndex;
                                      group_data = caches[groupAttrTableIndex].getData();
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
                              if (i == attributeExprs.size() - 1) {
                                  cout << attributeExpr.to_string() << "\n";
                              } else {
                                  cout << attributeExpr.to_string() << "\t";
                              }
                          }
                          attributeExpr.init_calculate();
                      }
                  }, indexExprs);

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
    int rc;
    rc = openTables(std::vector<std::string>{std::move(relationName)}, tables);
    if (rc != 0) {
        return rc;
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
    int rc;
    rc = openTables(std::vector<std::string>{std::move(relationName)}, tables);
    if (rc != 0) {
        return rc;
    }
    std::vector<int> attributeIndexs;
    rc = whereBindCheck(whereClause, tables);
    if (rc != 0) {
        return rc;
    }
    try {
        for (const auto &it: setClauses->clauses) {
            bindAttribute(it.second, tables);
            it.second->type_check();
            if (it.second->dataType == AttrType::NO_ATTR) {
                return QL_TYPE_CHECK;
            }
            int index = tables[0]->getColumnIndex(it.first->attribute);
            if (index < 0) {
                throw AttrBindException{"", it.first->attribute, EXPR_NO_SUCH_ATTRIBUTE};
            }
            const BindAttribute &info = tables[0]->getAttrInfo(index);
            if (info.attrType != it.second->dataType) {
                if (info.attrType == AttrType::FLOAT and it.second->dataType == AttrType::INT) {
                    it.second->convert_to_float();
                } else {
                    cerr << "Unsupported attribute assignment for column " << info.attrName << "\n";
                }
            }
            attributeIndexs.push_back(index);
        }
    } catch (AttrBindException &exception) {
        printException(exception);
        return QL_TABLE_FAIL;
    }
    std::vector<RID> toBeUpdated;
    iterateTables(*tables[0], whereClause,
                  [&toBeUpdated](const RM_Record &record) -> void {
                      toBeUpdated.push_back(record.getRID());
                  });
    for (const auto &rid: toBeUpdated) {
        RM_Record record;
        tables[0]->getFileHandler().getRec(rid, record);
        for (auto &it: setClauses->clauses) {
            it.second->init_calculate();
            it.second->calculate(record.getData());
        }
        tables[0]->updateData(record, attributeIndexs, setClauses);
    }
    return 0;
}

int QL_Manager::exeDelete(std::string relationName, Expr *whereClause) {
    std::vector<std::unique_ptr<Table>> tables;
    int rc;
    rc = openTables(std::vector<std::string>{std::move(relationName)}, tables);
    if (rc != 0) {
        return rc;
    }
    rc = whereBindCheck(whereClause, tables);
    if (rc != 0) {
        return rc;
    }
    std::vector<RID> toBeDeleted;
    iterateTables(*tables[0], whereClause,
                  [&toBeDeleted](const RM_Record &record) -> void {
                      toBeDeleted.push_back(record.getRID());
                  });
    RM_FileHandle &fileHandle = tables[0]->getFileHandler();
    for (auto &it: toBeDeleted) {
        rc = tables[0]->deleteData(it);
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

int QL_Manager::iterateTables(Table &table, Expr *condition, QL_Manager::CallbackFunc callback) {
    int rc;
    RM_FileScan fileScan;
    IX_IndexScan indexScan;
    bool use_index = false;
    Expr *current = condition;
    while (current != nullptr) {
        if (current->calculated or current->nodeType != NodeType::LOGIC_NODE or
            current->oper.logic != LogicOp::AND_OP) {
            break;
        }
        Expr *aim = current->right;
        if (not aim->calculated and aim->nodeType == NodeType::COMP_NODE) {
            // the left of comparison must be attribute
            if (aim->right->calculated) {
                if (aim->left->attrInfo.tableName == table.tableName &&
                    table.getIndexAvailable(aim->left->columnIndex)) {
                    indexScan.OpenScan(table.getIndexHandler(current->left->columnIndex), aim->oper.comp,
                                       aim->getValue());
                    use_index = true;
                    break;
                }
            }
        }
        current = current->left;
    }
    if (not use_index) {
        fileScan.openScan(table.getFileHandler(), nullptr, table.tableName);
    }
    while (true) {
        RM_Record record;
        if (use_index) {
            RID rid;
            rc = indexScan.GetNextEntry(rid);
            if (rc == 0) {
                table.getFileHandler().getRec(rid, record);
            }
        } else {
            rc = fileScan.getNextRec(record);
        }
        if (rc) {
            break;
        }
        condition->calculate(record.getData(), table.tableName);
        if (condition->is_true())
            callback(record);
        condition->init_calculate(table.tableName);
    }
    return 0;
}

void QL_Manager::bindAttribute(Expr *expr, const std::vector<std::unique_ptr<Table>> &tables) {
    expr->postorder([&tables](Expr *expr1) {
        if (expr1->nodeType == NodeType::ATTR_NODE) {
            if (!expr1->attribute->table.empty()) {
                int tableIndex = 0;
                for (const auto &table: tables) {
                    if (table->tableName == expr1->attribute->table) {
                        int index = table->getColumnIndex(expr1->attribute->attribute);
                        if (index >= 0) {
                            expr1->attrInfo = table->getAttrInfo(index);
                            expr1->dataType = expr1->attrInfo.attrType;
                            expr1->tableIndex = tableIndex;
                            expr1->columnIndex = index;
                            return;
                        } else {
                            throw AttrBindException{"", expr1->attribute->attribute, EXPR_NO_SUCH_ATTRIBUTE};
                        }
                    }
                    tableIndex += 1;
                }
                throw AttrBindException{expr1->attribute->table, "", EXPR_NO_SUCH_TABLE};
            } else {
                expr1->attrInfo.attrOffset = -1;
                int tableIndex = 0;
                for (const auto &table: tables) {
                    if (not expr1->attrInfo.tableName.empty()) {
                        throw AttrBindException{"", expr1->attribute->attribute, EXPR_AMBIGUOUS};
                    } else {
                        int index = table->getColumnIndex(expr1->attribute->attribute);
                        if (index >= 0) {
                            expr1->attrInfo = table->getAttrInfo(index);
                            expr1->dataType = expr1->attrInfo.attrType;
                            expr1->tableIndex = tableIndex;
                            expr1->columnIndex = index;
                        }
                    }
                    tableIndex += 1;
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

int QL_Manager::iterateTables(std::vector<std::unique_ptr<Table>> &tables, int current, Expr *condition,
                              QL_Manager::MultiTableFunc callback, std::list<Expr *> &indexExprs) {
    if (current == tables.size()) {
        callback(recordCaches);
        return 0;
    }
    int rc;
    RM_FileScan fileScan;
    IX_IndexScan indexScan;
    Table &table = *tables[current];
    bool use_index = false;
    if (!indexExprs.empty() and indexExprs.front()->left->tableIndex == current) {
        Expr *indexExpr = indexExprs.front();
        indexExprs.pop_front();
        indexScan.OpenScan(table.getIndexHandler(indexExpr->tableIndex), indexExpr->oper.comp,
                           indexExpr->right->getValue());
        use_index = true;
    } else {
        fileScan.openScan(table.getFileHandler(), nullptr, table.tableName);
    }
    while (true) {
        RM_Record record;
        if (use_index) {
            RID rid;
            rc = indexScan.GetNextEntry(rid);
            if (rc == 0) {
                if (table.getFileHandler().getRec(rid, record) != 0) {
                    cerr << "Can't find an entry in the index\n";
                    return -1;
                }
            }
        } else {
            rc = fileScan.getNextRec(record);
        }
        if (rc) {
            break;
        }
        condition->calculate(record.getData(), table.tableName);
        if (not condition->calculated or condition->is_true()) {
            recordCaches.push_back(std::move(record));
            iterateTables(tables, current + 1, condition, callback, indexExprs);
            recordCaches.pop_back();
        }
        condition->init_calculate(table.tableName);
    }
    return 0;
}
