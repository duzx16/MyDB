#include "sm.h"
#include "sm_internal.h"
#include <io.h>

#if defined(_WIN32)
#include <direct.h>
#include <cstring>
#elif defined(__linux__)
#include <sys/stat.h>
#include <sys/types.h>
#endif

SM_Manager::SM_Manager() {
	ixm = new IX_Manager(pfManager);
	rmm = &(RecordManager::getInstance());
	rmm->setPFManager(pfManager);
}

SM_Manager::~SM_Manager() {
	
}

RC SM_Manager::OpenDb(const char* dbName) {
	mkdir(dbName);
	chdir(dbName);
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
	
}

RC SM_Manager::CreateTable(const char *tableName, ColumnDecsList *columns, TableConstraintList *tableConstraints) {
	char s[1010];
	GenerateTableMetadataDir(tableName, s);
	pfManager.CreateFile(s);
	PF_FileHandle fileHandle;
	pfManager.OpenFile(s, fileHandle);
	PF_PageHandle pageHandle;
	fileHandle.AllocatePage(pageHandle);
	char *pageData;
	pageHandle.GetData(pageData);
	TableInfo *tableInfo = (TableInfo*) pageData;
	tableInfo->attrInfoCount = columns->getColumnCount();
	AttrInfo *attrInfos = columns->getAttrInfos();
	for (int i = 0; i < tableInfo->attrInfoCount; ++i) {
		tableInfo->attrInfos[i] = attrInfos[i];
	}
	columns->deleteAttrInfos();
	tableInfo->tableConsCount = (tableConstraints->tbDecs).size();
	for (int i = 0; i < tableInfo->tableConsCount; ++i) {
		TableCons *tableCons = &(tableInfo->tableCons[i]);
		TableConstraint *tableConsFrom = tableConstraints->tbDecs[i];
		tableCons->type = tableConsFrom->type;
		tableCons->column_name = tableConsFrom->column_name;
		tableCons->foreign_table = tableConsFrom->foreign_table;
		tableCons->foreign_column = tableConsFrom->foreign_column;
		if (tableConsFrom->const_values != nullptr) {
			for (auto expr : tableConsFrom->const_values->constValues) {
				// insert Expr expr into exprs
				tableCons->exprs[tableCons->exprSize++] = getExprTreeFromExpr(expr);
			}
		}
		if (tableConsFrom->column_list != nullptr) {
			for (auto _ : tableConsFrom->column_list->idents) {
				(tableCons->column_list).addIdent(_.c_str());
			}
		}
	}
	PageNum pageNum;
	pageHandle.GetPageNum(pageNum);
	fileHandle.ForcePages(pageNum);
}

RC SM_Manager::GetTableInfo(const char *tableName, ColumnDecsList &columns, TableConstraintList &tableConstraints) {
	char s[1010];
	GenerateTableMetadataDir(tableName, s);
	PF_FileHandle fileHandle;
	pfManager.OpenFile(s, fileHandle);
	PF_PageHandle pageHandle;
	fileHandle.GetThisPage(0, pageHandle);
	char *pageData;
	pageHandle.GetData(pageData);
	TableInfo *tableInfo = (TableInfo*) pageData;
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
		TableCons tableCons = tableInfo->tableCons[i];
		if (tableCons.type == ConstraintType::PRIMARY_CONSTRAINT) {
			tableConstraints.addTbDec(
				new TableConstraint(&(tableCons.column_list))
			);
		}
		else if (tableCons.type == ConstraintType::FOREIGN_CONSTRAINT) {
			tableConstraints.addTbDec(
				new TableConstraint(
					tableCons.column_name.c_str(),
					tableCons.foreign_table.c_str(),
					tableCons.foreign_column.c_str()
				)
			);
		}
		else if (tableCons.type == ConstraintType::CHECK_CONSTRAINT) {
			ConstValueList *const_values = new ConstValueList();
			for (int j = 0; j < tableCons.exprSize; ++j)
				const_values->addConstValue(getExprFromExprTree(tableCons.exprs[j]));
			tableConstraints.addTbDec(
				new TableConstraint(
					tableCons.column_name.c_str(),
					const_values
				)
			);
		}
	}
}

RC SM_Manager::DropTable(const char *relName) {
	
}

RC SM_Manager::CreateIndex(const char *relName, const char *attrName) {
	char s[1010];
	GenerateTableMetadataDir(relName, s);
	PF_FileHandle fileHandle;
	pfManager.OpenFile(s, fileHandle);
	PF_PageHandle pageHandle;
	fileHandle.GetThisPage(0, pageHandle);
	char *pageData;
	pageHandle.GetData(pageData);
	TableInfo *tableInfo = (TableInfo*) pageData;
	for (int i = 0; i < tableInfo->indexedAttrSize; ++i) {
		if (strcmp((tableInfo->attrInfos[tableInfo->indexedAttr[i]]).attrName, attrName) == 0) {
			// index existed
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
		return SM_INDEX_NOTEXIST;
	}
	// create index
	tableInfo->indexedAttr[tableInfo->indexedAttrSize++] = pos;
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
	}
	fileHandle.ForcePages(0);
	return 0;
}

RC SM_Manager::DropIndex(const char *relName, const char *attrName) {
	char s[1010];
	GenerateTableMetadataDir(relName, s);
	PF_FileHandle fileHandle;
	pfManager.OpenFile(s, fileHandle);
	PF_PageHandle pageHandle;
	fileHandle.GetThisPage(0, pageHandle);
	char *pageData;
	pageHandle.GetData(pageData);
	TableInfo *tableInfo = (TableInfo*) pageData;
	// find indexed pos
	int pos = -1;
	for (int i = 0; i < tableInfo->indexedAttrSize; ++i) {
		if (strcmp((tableInfo->attrInfos[tableInfo->indexedAttr[i]]).attrName, attrName) == 0) {
			pos = i;
			break;
		}
	}
	if (pos == -1) {
		return SM_INDEX_NOTEXIST;
	}
	// remove index file
	ixm->DestroyIndex(relName, tableInfo->indexedAttr[pos]);
	// delete this pos in indexed arr
	for (int i = pos; i <= tableInfo->indexedAttrSize - 2; ++i)
		tableInfo->indexedAttr[i] = tableInfo->indexedAttr[i + 1];
	--tableInfo->indexedAttrSize;
	fileHandle.ForcePages(0);
	return 0;
}

RC SM_Manager::Load(const char *relName, const char *fileName) {
	
}

RC SM_Manager::Help() {
	
}

RC SM_Manager::Help(const char *relName) {
	
}

RC SM_Manager::Print(const char *relName) {
	// print table attrInfo
	char s[1010];
	GenerateTableMetadataDir(relName, s);
	PF_FileHandle fileHandle;
	pfManager.OpenFile(s, fileHandle);
	PF_PageHandle pageHandle;
	fileHandle.GetThisPage(0, pageHandle);
	char *pageData;
	pageHandle.GetData(pageData);
	TableInfo *tableInfo = (TableInfo*) pageData;
	
}