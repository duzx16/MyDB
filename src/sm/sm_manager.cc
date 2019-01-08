#include "sm.h"
#include "sm_internal.h"
#include <cstring>
#include <cstdio>
#include <cassert>
#include <unistd.h>

#if defined(_WIN32)
#include <io.h>
#include <direct.h>
#elif defined(__linux__) or defined(__APPLE__)
#include <sys/stat.h>
#include <sys/types.h>
#endif

#define LDB(res) Debug(__FILE__, __LINE__, (res))
#define CHKL printf("line %d is ok\n", __LINE__)

SM_Manager::SM_Manager(): pfManager(PF_Manager::getInstance()) {
    ixm = &IX_Manager::getInstance();
    rmm = &(RecordManager::getInstance());
}

SM_Manager::~SM_Manager() = default;

RC SM_Manager::CreateDb(const char *dbName) {
#if defined(_WIN32)
    mkdir(dbName);
#else
    mkdir(dbName, S_IRWXU);
#endif
    chdir(dbName);
    LDB(pfManager.CreateFile("TableList"));
    PF_FileHandle fileHandle;
    LDB(pfManager.OpenFile("TableList", fileHandle));
    PF_PageHandle pageHandle;
    LDB(fileHandle.AllocatePage(pageHandle));
    PageNum pageNum;
    LDB(pageHandle.GetPageNum(pageNum));
    printf("TableList pageNum = %ld\n", pageNum);
    fileHandle.UnpinPage(pageNum);
    LDB(pfManager.CloseFile(fileHandle));
    chdir("..");
    return 0;
}

RC SM_Manager::DropDb(const char *dbName) {
    char s[1010];
#if defined(__linux__) or defined(__APPLE__)
    sprintf(s, "rm -r %s", dbName);
#elif defined(_WIN32)
    sprintf(s, "rd /s/q %s", dbName);
#endif
    return system(s);
}

RC SM_Manager::OpenDb(const char *dbName) {
    return chdir(dbName);
}

void SM_Manager::GenerateTableMetadataDir(const char *tableName, char *s) {
    strcat(s, tableName);
    strcat(s, "_Metadata");
}

void SM_Manager::GenerateTableRecordDir(const char *tableName, char *s) {
    strcat(s, tableName);
    strcat(s, "_Record");
}

RC SM_Manager::CloseDb() {
    return chdir("..");
}

RC SM_Manager::CreateTable(const char *tableName, ColumnDecsList *columns, TableConstraintList *tableConstraints) {
    //printf("SM_Manager::CreateTable\n");
    char s[1010] = {};
    GenerateTableMetadataDir(tableName, s);
    //printf("name = %s\n", s);
    if (pfManager.CreateFile(s) != 0) {
        return SM_REL_EXISTS;
    }
    PF_FileHandle fileHandle;
    LDB(pfManager.OpenFile(s, fileHandle));
    PF_PageHandle pageHandle;
    LDB(fileHandle.AllocatePage(pageHandle));
    char *pageData;
    LDB(pageHandle.GetData(pageData));
    //printf("allocating page\n");
    auto *tableInfo = (TableInfo *) pageData;
    tableInfo->attrInfoCount = columns->getColumnCount();
    AttrInfo *attrInfos = columns->getAttrInfos();
    for (int i = 0; i < tableInfo->attrInfoCount; ++i) {
        tableInfo->attrInfos[i] = attrInfos[i];
    }
    columns->deleteAttrInfos();
    //printf("attr inserted\n");

    if (tableConstraints != nullptr) {
        //printf("tableConstraints not nullptr!\n");
        tableInfo->tableConsCount = (tableConstraints->tbDecs).size();


        //printf("tableConsCount = %d\n", tableInfo->tableConsCount);
        for (int i = 0; i < tableInfo->tableConsCount; ++i) {
            // allocate a new page to store a TableCons, pageNum = i + 1
            PF_PageHandle tableConsPageHandle;
            LDB(fileHandle.AllocatePage(tableConsPageHandle));
            char *tableConsPageData;
            LDB(tableConsPageHandle.GetData(tableConsPageData));

            auto *tableCons = (TableCons *) tableConsPageData;
            TableConstraint *tableConsFrom = tableConstraints->tbDecs[i];
            tableCons->type = tableConsFrom->type;
            //printf("hello?\n");
            if (!tableConsFrom->column_name.empty()) {
                //CHKL;
                //printf("column_name = %s\n", (tableConsFrom->column_name).c_str());
                //printf("name_length = %d\n", strlen((tableConsFrom->column_name).c_str()));
                strcpy(tableCons->column_name, (tableConsFrom->column_name).c_str());
                //memcpy(tableCons->column_name, (tableConsFrom->column_name).c_str(), sizeof (tableCons->column_name));
                //CHKL;
            }
            if (!tableConsFrom->foreign_table.empty()) {
                //CHKL;
                strcpy(tableCons->foreign_table, (tableConsFrom->foreign_table).c_str());
            }
            if (!tableConsFrom->foreign_column.empty()) {
                //CHKL;
                strcpy(tableCons->foreign_column, (tableConsFrom->foreign_column).c_str());
            }
            //printf("hey man\n");
            if (tableConsFrom->const_values != nullptr && tableConsFrom->const_values != NULL) {
                //printf("const_values not null!\n");
                for (Expr *expr : tableConsFrom->const_values->constValues) {
                    if (expr == nullptr) {
                        //printf("expr null!\n");
                    }
                    // insert Expr expr into exprs
                    ConstNode *constNode = getConstNodeFromExpr(expr);
                    //CHKL;
                    tableCons->constNodes[tableCons->constSize++] = *constNode;
                    //CHKL;
                    delete constNode;
                }
                //printf("const_values inserted\n");
            }
            if (tableConsFrom->column_list != nullptr) {
                //printf("column_list not null!\n");
                for (const auto &_ : tableConsFrom->column_list->idents) {
                    (tableCons->columnList).addColumn(_.c_str());
                }
                //printf("column_list inserted\n");
            }

            PageNum tableConsPageNum;
            LDB(tableConsPageHandle.GetPageNum(tableConsPageNum));
            assert(tableConsPageNum == i + 1);
            LDB(fileHandle.MarkDirty(tableConsPageNum));
            LDB(fileHandle.ForcePages(tableConsPageNum));
            LDB(fileHandle.UnpinPage(tableConsPageNum));
        }
    }
    //printf("cons inserted\n");

    PageNum pageNum;
    LDB(pageHandle.GetPageNum(pageNum));
    assert(pageNum == 0);
    LDB(fileHandle.MarkDirty(pageNum));
    LDB(fileHandle.ForcePages(pageNum));
    LDB(fileHandle.UnpinPage(pageNum));
    LDB(pfManager.CloseFile(fileHandle));

    LDB(pfManager.OpenFile("TableList", fileHandle));
    LDB(fileHandle.GetThisPage(0, pageHandle));
    LDB(pageHandle.GetData(pageData));
    auto *tableList = (TableList *) pageData;
    //printf("pre count = %d\n", tableList->tableCount);
    //printf("tableName = %s\n", tableName);
    ++tableList->tableCount;
    //printf("aft count = %d\n", tableList->tableCount);
    strcpy(tableList->tables[tableList->tableCount - 1], tableName);
    //printf("OK\n");
    LDB(fileHandle.MarkDirty(0));
    LDB(fileHandle.ForcePages(0));
    LDB(fileHandle.UnpinPage(0));
    LDB(pfManager.CloseFile(fileHandle));
    //printf("page updated\n");
    //printf("~SM_Manager::CreateTable\n");
    return 0;
}

RC SM_Manager::GetTableInfo(const char *tableName, ColumnDecsList &columns, TableConstraintList &tableConstraints) {
    char s[1010] = {};
    GenerateTableMetadataDir(tableName, s);
    PF_FileHandle fileHandle;
    if (pfManager.OpenFile(s, fileHandle) != 0) {
        return SM_REL_NOTEXIST;
    }
    PF_PageHandle pageHandle;
    LDB(fileHandle.GetThisPage(0, pageHandle));
    char *pageData;
    LDB(pageHandle.GetData(pageData));
    auto *tableInfo = (TableInfo *) pageData;
    for (int i = 0; i < tableInfo->attrInfoCount; ++i) {
        AttrInfo attrInfo = tableInfo->attrInfos[i];
        columns.addColumn(
                new ColumnNode(
                        attrInfo.attrName,
                        attrInfo.attrType,
                        attrInfo.attrLength,
                        attrInfo.notNull
                )
        );
    }
    for (int i = 0; i < tableInfo->tableConsCount; ++i) {
        PF_PageHandle tableConsPageHandle;
        fileHandle.GetThisPage(i + 1, tableConsPageHandle);
        char *tableConsPageData;
        tableConsPageHandle.GetData(tableConsPageData);
        auto *tableCons = (TableCons *) tableConsPageData;
        //TableCons tableCons = tableInfo->tableCons[i];
        if (tableCons->type == ConstraintType::PRIMARY_CONSTRAINT) {
            tableConstraints.addTbDec(
                    //new TableConstraint(&(tableCons.column_list))
                    new TableConstraint(getIdentListFromColumnList(&(tableCons->columnList)))
            );
        } else if (tableCons->type == ConstraintType::FOREIGN_CONSTRAINT) {
            tableConstraints.addTbDec(
                    new TableConstraint(
                            tableCons->column_name,
                            tableCons->foreign_table,
                            tableCons->foreign_column
                    )
            );
        } else if (tableCons->type == ConstraintType::CHECK_CONSTRAINT) {
            auto *const_values = new ConstValueList();
            for (int j = 0; j < tableCons->constSize; ++j)
                const_values->addConstValue(getExprFromConstNode(&(tableCons->constNodes[j])));
            tableConstraints.addTbDec(
                    new TableConstraint(
                            tableCons->column_name,
                            const_values
                    )
            );
        }
        LDB(fileHandle.UnpinPage(i + 1));
    }
    LDB(fileHandle.UnpinPage(0));
    LDB(pfManager.CloseFile(fileHandle));
    return 0;
}

RC SM_Manager::DropTable(const char *relName) {

}

RC SM_Manager::CreateIndex(const char *relName, const char *attrName) {
    char s[1010] = {};
    GenerateTableMetadataDir(relName, s);
    PF_FileHandle fileHandle;
    pfManager.OpenFile(s, fileHandle);
    PF_PageHandle pageHandle;
    fileHandle.GetThisPage(0, pageHandle);
    char *pageData;
    pageHandle.GetData(pageData);
    auto *tableInfo = (TableInfo *) pageData;
    for (int i = 0; i < tableInfo->indexedAttrSize; ++i) {
        if (strcmp((tableInfo->attrInfos[tableInfo->indexedAttr[i]]).attrName, attrName) == 0) {
            // index existed
            pfManager.CloseFile(fileHandle);
            return SM_INDEX_EXISTS;
        }
    }
    int pos = -1;
    for (int i = 0; i < tableInfo->attrInfoCount; ++i) {
        if (strcmp((tableInfo->attrInfos[i]).attrName, attrName) == 0) {
            pos = i;
            break;
        }
    }
    if (pos == -1) {
        // index name not found
        pfManager.CloseFile(fileHandle);
        return SM_INDEX_NOTEXIST;
    }
    tableInfo->indexedAttr[tableInfo->indexedAttrSize++] = pos;
    fileHandle.ForcePages();
    pfManager.CloseFile(fileHandle);
    // create index
    ixm->CreateIndex(relName, pos, (tableInfo->attrInfos[pos]).attrType, (tableInfo->attrInfos[pos]).attrLength);
    IX_IndexHandle indexHandle;
    ixm->OpenIndex(relName, pos, indexHandle);
    // calc offset
    int offset = 0;
    for (int i = 0; i < pos; ++i)
        offset += (tableInfo->attrInfos[i]).attrLength;
    // open record scan
    RM_FileHandle rmFileHandle;
    memset(s, 0, sizeof(s));
    GenerateTableRecordDir(relName, s);
    if (rmm->openFile(s, rmFileHandle) == 0) {
        RM_FileScan rmFileScan;
        rmFileScan.openScan(
                rmFileHandle,
                (tableInfo->attrInfos[pos]).attrType,
                (tableInfo->attrInfos[pos]).attrLength,
                offset,
                CompOp::NO_OP,
                nullptr
        );
        RM_Record rec;
        while (rmFileScan.getNextRec(rec) == 0) {
            // insert entry into index
            char *pdata = rec.getData();
            RID rid = rec.getRID();
            indexHandle.InsertEntry(pdata, rid);
        }
        indexHandle.ForcePages();
        rmm->closeFile(rmFileHandle);
    }
    return 0;
}

RC SM_Manager::DropIndex(const char *relName, const char *attrName) {
    char s[1010] = {};
    GenerateTableMetadataDir(relName, s);
    PF_FileHandle fileHandle;
    pfManager.OpenFile(s, fileHandle);
    PF_PageHandle pageHandle;
    fileHandle.GetThisPage(0, pageHandle);
    char *pageData;
    pageHandle.GetData(pageData);
    auto *tableInfo = (TableInfo *) pageData;
    // find indexed pos
    int pos = -1;
    for (int i = 0; i < tableInfo->indexedAttrSize; ++i) {
        if (strcmp((tableInfo->attrInfos[tableInfo->indexedAttr[i]]).attrName, attrName) == 0) {
            pos = i;
            break;
        }
    }
    if (pos == -1) {
        pfManager.CloseFile(fileHandle);
        return SM_INDEX_NOTEXIST;
    }
    // remove index file
    LDB(ixm->DestroyIndex(relName, tableInfo->indexedAttr[pos]));
    // delete this pos in indexed arr
    for (int i = pos; i <= tableInfo->indexedAttrSize - 2; ++i)
        tableInfo->indexedAttr[i] = tableInfo->indexedAttr[i + 1];
    --tableInfo->indexedAttrSize;
    fileHandle.ForcePages(0);
    pfManager.CloseFile(fileHandle);
    return 0;
}

RC SM_Manager::Load(const char *relName, const char *fileName) {

}

RC SM_Manager::Help() {
    //printf("show tables; in\n");
    PF_FileHandle fileHandle;
    LDB(pfManager.OpenFile("TableList", fileHandle));
    PF_PageHandle pageHandle;
    LDB(fileHandle.GetThisPage(0, pageHandle));
    char *pageData;
    LDB(pageHandle.GetData(pageData));
    auto *tableList = (TableList *) pageData;
    //printf("tableCount = %d\n", tableList->tableCount);
    for (int i = 0; i < tableList->tableCount; ++i)
        Help(tableList->tables[i]);
    LDB(fileHandle.UnpinPage(0));
    LDB(pfManager.CloseFile(fileHandle));
    //printf("show tables; out\n");
}

RC SM_Manager::Help(const char *relName) {
    // print table name
    printf("Table: %s\n", relName);
    // print table attrInfo
    char s[1010] = {};
    GenerateTableMetadataDir(relName, s);
    PF_FileHandle fileHandle;
    LDB(pfManager.OpenFile(s, fileHandle));
    PF_PageHandle pageHandle;
    LDB(fileHandle.GetThisPage(0, pageHandle));
    char *pageData;
    LDB(pageHandle.GetData(pageData));
    auto *tableInfo = (TableInfo *) pageData;
    printf("%d Attributes:\n", tableInfo->attrInfoCount);
    for (int i = 0; i < tableInfo->attrInfoCount; ++i) {
        printf("\t");
        AttrInfo *attrInfo = &(tableInfo->attrInfos[i]);
        printf("%s ", attrInfo->attrName);
        printAttrType(attrInfo->attrType);
        printf(" length = %d\n", attrInfo->attrLength);
    }

    printf("%d Constraints:\n", tableInfo->tableConsCount);
    for (int i = 0; i < tableInfo->tableConsCount; ++i) {
        PF_PageHandle tableConsPageHandle;
        LDB(fileHandle.GetThisPage(i + 1, tableConsPageHandle));
        char *tableConsPageData;
        LDB(tableConsPageHandle.GetData(tableConsPageData));

        printf("\t");
        auto *tableCons = (TableCons *) tableConsPageData;
        printConstraintType(tableCons->type);
        if (tableCons->type == ConstraintType::PRIMARY_CONSTRAINT) {
            printf("column_list =");
            for (int i = 0; i < (tableCons->columnList).columnCount; ++i)
                printf(" %s,", (tableCons->columnList).columns[i]);
            //for (std::string ident : (tableCons->column_list).idents)
            //	printf(" %s,", ident);
            printf("\n");
        } else if (tableCons->type == ConstraintType::FOREIGN_CONSTRAINT) {
            printf(" column_name = %s", tableCons->column_name);
            printf(" foreign_table = %s", tableCons->foreign_table);
            printf(" foreign_column = %s\n", tableCons->foreign_column);
        } else if (tableCons->type == ConstraintType::CHECK_CONSTRAINT) {
            printf(" column_name = %s", tableCons->column_name);
            printf(" value list = {");
            for (int j = 0; j < tableCons->constSize; ++j) {
                Expr *expr = getExprFromConstNode(&(tableCons->constNodes[j]));
                //printf("%d, %s", __LINE__, (expr->value_s).c_str());
                printf(" ");
                printConstNodeExpr(expr);
                delete expr;
            }
            printf(" }\n");
        }

        LDB(fileHandle.UnpinPage(i + 1));
    }

    LDB(fileHandle.UnpinPage(0));
    LDB(pfManager.CloseFile(fileHandle));
    return 0;
}

SM_Manager *SM_Manager::getInstance() {
    static SM_Manager instance;
    return &instance;
}