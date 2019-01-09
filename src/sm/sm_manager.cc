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


#define CHKL printf("line %d is ok\n", __LINE__)

SM_Manager::SM_Manager() : pfManager(PF_Manager::getInstance()) {
    ixm = &IX_Manager::getInstance();
    rmm = &RecordManager::getInstance();
}

SM_Manager::~SM_Manager() = default;

RC SM_Manager::CreateDb(const char *dbName) {
    RC rc;
#if defined(_WIN32)
    rc = mkdir(dbName);
#else
    rc = mkdir(dbName, S_IRWXU);
#endif
    if (rc != 0)
        return rc;
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
    char s[1010] = {};
    GenerateTableMetadataDir(tableName, s);
    if (pfManager.CreateFile(s) != 0) {
        return SM_REL_EXISTS;
    }

    // check if foreign cons are valid
    if (tableConstraints != nullptr) {
        for (int i = 0; i < (tableConstraints->tbDecs).size(); ++i) {
            TableConstraint *tableConsFrom = tableConstraints->tbDecs[i];
            if (tableConsFrom->type == ConstraintType::FOREIGN_CONSTRAINT) {
                // check valid
                if (!foreignTableExist((tableConsFrom->foreign_table).c_str())) {
                    printf("SM_FOREIGN_REL_NOT_FOUND\n");
                    return SM_FOREIGN_REL_NOT_FOUND;
                }
                char foreignTableFile[1010] = {};
                GenerateTableMetadataDir((tableConsFrom->foreign_table).c_str(), foreignTableFile);
                PF_FileHandle foreignTableFileHandle;
                if (pfManager.OpenFile(foreignTableFile, foreignTableFileHandle) != 0)
                    return SM_FOREIGN_REL_NOT_FOUND;
                PF_PageHandle pageHandle;
                LDB(foreignTableFileHandle.GetThisPage(0, pageHandle));
                char *pageData;
                LDB(pageHandle.GetData(pageData));
                TableInfo *tableInfo = (TableInfo *) pageData;
                bool found = false;
                for (int i = 0; i < tableInfo->tableConsCount; ++i) {
                    LDB(foreignTableFileHandle.GetThisPage(i + 1, pageHandle));
                    LDB(pageHandle.GetData(pageData));
                    TableCons *tableCons = (TableCons *) pageData;
                    if (tableCons->type == ConstraintType::PRIMARY_CONSTRAINT) {
                        for (int j = 0; j < (tableCons->columnList).columnCount; ++j) {
                            if (strcmp((tableCons->columnList).columns[j], (tableConsFrom->foreign_column).c_str()) ==
                                0)
                                found = true;
                        }
                    }
                    LDB(foreignTableFileHandle.UnpinPage(i + 1));
                    if (found)
                        break;
                }
                LDB(foreignTableFileHandle.UnpinPage(0));
                LDB(pfManager.CloseFile(foreignTableFileHandle));
                if (!found) {
                    printf("SM_FOREIGN_KEY_NOT_FOUND\n");
                    return SM_FOREIGN_KEY_NOT_FOUND;
                }
            }
        }
    }

    //printf("SM_Manager::CreateTable\n");
    PF_FileHandle fileHandle;
    LDB(pfManager.OpenFile(s, fileHandle));
    PF_PageHandle pageHandle;
    LDB(fileHandle.AllocatePage(pageHandle));
    char *pageData;
    LDB(pageHandle.GetData(pageData));
    auto *tableInfo = (TableInfo *) pageData;
    tableInfo->attrInfoCount = columns->getColumnCount();
    AttrInfo *attrInfos = columns->getAttrInfos();
    for (int i = 0; i < tableInfo->attrInfoCount; ++i) {
        tableInfo->attrInfos[i] = attrInfos[i];
    }
    columns->deleteAttrInfos();

    if (tableConstraints != nullptr) {
        tableInfo->tableConsCount = (tableConstraints->tbDecs).size();
        for (int i = 0; i < tableInfo->tableConsCount; ++i) {
            TableConstraint *tableConsFrom = tableConstraints->tbDecs[i];
            // allocate a new page to store a TableCons, pageNum = i + 1
            PF_PageHandle tableConsPageHandle;
            LDB(fileHandle.AllocatePage(tableConsPageHandle));
            char *tableConsPageData;
            LDB(tableConsPageHandle.GetData(tableConsPageData));

            auto *tableCons = (TableCons *) tableConsPageData;

            tableCons->type = tableConsFrom->type;
            if (!tableConsFrom->column_name.empty()) {
                strcpy(tableCons->column_name, (tableConsFrom->column_name).c_str());
            }
            if (!tableConsFrom->foreign_table.empty()) {
                strcpy(tableCons->foreign_table, (tableConsFrom->foreign_table).c_str());
            }
            if (!tableConsFrom->foreign_column.empty()) {
                strcpy(tableCons->foreign_column, (tableConsFrom->foreign_column).c_str());
            }
            if (tableConsFrom->const_values != nullptr && tableConsFrom->const_values != NULL) {
                for (Expr *expr : tableConsFrom->const_values->constValues) {
                    ConstNode *constNode = getConstNodeFromExpr(expr);
                    tableCons->constNodes[tableCons->constSize++] = *constNode;
                    delete constNode;
                }
            }
            if (tableConsFrom->column_list != nullptr) {
                for (const auto &_ : tableConsFrom->column_list->idents) {
                    (tableCons->columnList).addColumn(_.c_str());
                }
            }

            PageNum tableConsPageNum;
            LDB(tableConsPageHandle.GetPageNum(tableConsPageNum));
            //printf("i = %d, tableConsPageNum = %d\n", i, tableConsPageNum);
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
    ++tableList->tableCount;
    strcpy(tableList->tables[tableList->tableCount - 1], tableName);
    LDB(fileHandle.MarkDirty(0));
    LDB(fileHandle.ForcePages(0));
    LDB(fileHandle.UnpinPage(0));
    LDB(pfManager.CloseFile(fileHandle));
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
                        attrInfo.columnFlag
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
    RC rc;
    // get table metadata
    char s[1010] = {};
    GenerateTableMetadataDir(relName, s);
    PF_FileHandle fileHandle;
    if ((rc = pfManager.OpenFile(s, fileHandle)) != 0)
        return rc;
    PF_PageHandle pageHandle;
    LDB(fileHandle.GetThisPage(0, pageHandle));
    char *pageData;
    LDB(pageHandle.GetData(pageData));
    TableInfo *tableInfo = (TableInfo *) pageData;
    // delete all index files
    for (int i = 0; i < tableInfo->indexedAttrSize; ++i) {
        if ((rc = ixm->DestroyIndex(relName, tableInfo->indexedAttr[i])) != 0)
            return rc;
    }
    // delete table metadata file
    if ((rc = pfManager.DestroyFile(s)) != 0)
        return rc;
    // delete table rec file
    GenerateTableRecordDir(relName, s);
    std::string recFileName(s);
    if ((rc = rmm->destroyFile(recFileName)) != 0)
        return rc;
    return 0;
}

RC SM_Manager::CreateIndex(const char *relName, const char *attrName) {
    RC rc;
    char s[1010] = {};
    GenerateTableMetadataDir(relName, s);
    PF_FileHandle fileHandle;
    if ((rc = pfManager.OpenFile(s, fileHandle)) != 0)
        return rc;
    PF_PageHandle pageHandle;
    LDB(fileHandle.GetThisPage(0, pageHandle));
    char *pageData;
    LDB(pageHandle.GetData(pageData));
    auto *tableInfo = (TableInfo *) pageData;
    for (int i = 0; i < tableInfo->indexedAttrSize; ++i) {
        if (strcmp((tableInfo->attrInfos[tableInfo->indexedAttr[i]]).attrName, attrName) == 0) {
            // index existed
            LDB(pfManager.CloseFile(fileHandle));
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
        LDB(fileHandle.UnpinPage(0));
        LDB(pfManager.CloseFile(fileHandle));
        return SM_INDEX_NOTEXIST;
    }
    tableInfo->indexedAttr[tableInfo->indexedAttrSize++] = pos;
	LDB(fileHandle.MarkDirty(0));
    // create index
    if ((rc = ixm->CreateIndex(relName, pos, (tableInfo->attrInfos[pos]).attrType,
                               (tableInfo->attrInfos[pos]).attrSize)) != 0)
        return rc;
    // calc offset
    int offset = 1;
    for (int i = 0; i < pos; ++i) {
        offset += (tableInfo->attrInfos[i]).attrSize + 1;
    }
    // open record scan
    RM_FileHandle rmFileHandle;
    memset(s, 0, sizeof(s));
    GenerateTableRecordDir(relName, s);
    if ((rc = rmm->openFile(s, rmFileHandle)) != 0)
        return rc;
    IX_IndexHandle indexHandle;
    if ((rc = ixm->OpenIndex(relName, pos, indexHandle, rmFileHandle, offset)) != 0)
        return rc;
    RM_FileScan rmFileScan;
    if ((rc = rmFileScan.openScan(
            rmFileHandle,
            (tableInfo->attrInfos[pos]).attrType,
            (tableInfo->attrInfos[pos]).attrSize,
            offset,
            CompOp::ISNOT_OP,
            nullptr
    )) != 0)
        return rc;
    RM_Record rec;
    while (rmFileScan.getNextRec(rec) == 0) {
        indexHandle.InsertEntry(rec.getRID());
    }
    LDB(ixm->CloseIndex(indexHandle));
    LDB(rmm->closeFile(rmFileHandle));
    LDB(fileHandle.ForcePages(0));
    LDB(fileHandle.UnpinPage(0));
    LDB(pfManager.CloseFile(fileHandle));
    return 0;
}

RC SM_Manager::DropIndex(const char *relName, const char *attrName) {
    RC rc;
    char s[1010] = {};
    GenerateTableMetadataDir(relName, s);
    PF_FileHandle fileHandle;
    if ((rc = pfManager.OpenFile(s, fileHandle)) != 0)
        return rc;
    PF_PageHandle pageHandle;
    LDB(fileHandle.GetThisPage(0, pageHandle));
    char *pageData;
    LDB(pageHandle.GetData(pageData));
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
        LDB(fileHandle.UnpinPage(0));
        LDB(pfManager.CloseFile(fileHandle));
        return SM_INDEX_NOTEXIST;
    }
    // remove index file
    LDB(ixm->DestroyIndex(relName, tableInfo->indexedAttr[pos]));
    // delete this pos in indexed arr
    for (int i = pos; i <= tableInfo->indexedAttrSize - 2; ++i)
        tableInfo->indexedAttr[i] = tableInfo->indexedAttr[i + 1];
    --tableInfo->indexedAttrSize;
    LDB(fileHandle.MarkDirty(0));
    LDB(fileHandle.ForcePages(0));
	LDB(fileHandle.UnpinPage(0));
    LDB(pfManager.CloseFile(fileHandle));
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
    return 0;
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
        printf(" length = %d\n", attrInfo->attrSize);
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

bool SM_Manager::foreignTableExist(const char *relName) {
    PF_FileHandle fileHandle;
    LDB(pfManager.OpenFile("TableList", fileHandle));
    PF_PageHandle pageHandle;
    LDB(fileHandle.GetThisPage(0, pageHandle));
    char *pageData;
    LDB(pageHandle.GetData(pageData));
    TableList *tableList = (TableList *) pageData;
    bool found = false;
    for (int i = 0; i < tableList->tableCount; ++i) {
        if (strcmp(tableList->tables[i], relName) == 0) {
            found = true;
            break;
        }
    }
    LDB(fileHandle.UnpinPage(0));
    LDB(pfManager.CloseFile(fileHandle));
    return found;
}
