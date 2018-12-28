//
// Created by Zhengxiao Du on 2018-12-20.
//

#ifndef MYDB_TABLE_H
#define MYDB_TABLE_H

#include "../parser/Tree.h"
#include "../rm/RM_FileHandle.h"
#include "../rm/RecordManager.h"
#include "../rm/RID.h"
#include "../ix/ix.h"
#include "../parser/Expr.h"

#include <vector>

class Table {
public:
    explicit Table(const std::string &tableName);

    ~Table();

    int getOffset(const std::string &attribute) const;

    int getColumnIndex(const std::string &attribute) const;

    ColumnNode *getColumn(const std::string &attribute) const;

    std::string printData(const char *data);

    std::string checkData(char *data);

    int deleteData(const RID &rid);

    int insertData(const IdentList *columnList, const ConstValueList *constValues);

    int updateData(const RM_Record &record, const std::vector<int> &attrIndexes, SetClauseList *setClauses);

    int insertIndex(char *data, const RID &rid);

    RM_FileHandle &getFileHandler();

    std::string tableName;
private:
    int tryOpenFile();

    int tryOpenIndex(int indexNo);

    std::vector<BindAttribute> attrInfos;
    int recordSize;

    ColumnDecsList columns;
    TableConstraintList tableConstraints;
    RM_FileHandle fileHandle;
    std::vector<IX_IndexHandle *> indexHandles;

};


#endif //MYDB_TABLE_H
