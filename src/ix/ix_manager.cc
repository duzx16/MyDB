#include "ix.h"
#include "ix_internal.h"
#include <cstring>
#include <cstdio>

IX_Manager::IX_Manager(PF_Manager *pfm) {
	this->pfm = pfm;
}

IX_Manager::~IX_Manager() {}

char* IX_Manager::GenFileName(const char* filename, int indexNo) {
	int len = strlen(filename);
	char *file = new char[len + 15];
	strcpy(file, filename);
	file[len++] = '.';
	if (indexNo == 0)
		file[len++] = '0';
	while (indexNo > 0) {
		file[len++] = '0' + (indexNo % 10);
		indexNo /= 10;
	}
	file[len++] = '\0';
	return file;
}

RC IX_Manager::CreateIndex(const char* filename, int indexNo, AttrType attrType, int attrLength) {
	// gen filename
	RC rc;
	char *file = GenFileName(filename, indexNo);
	// open file and allocate page 0 : IndexInfo
	if ((rc = pfm->CreateFile(file)) != 0)
		return rc;
	PF_FileHandle fileHandle;
	if ((rc = pfm->OpenFile(file, fileHandle)) != 0)
		return rc;
	PF_PageHandle pageHandle;
	LDB(fileHandle.AllocatePage(pageHandle));
	char *pageData;
	LDB(pageHandle.GetData(pageData));
	PageNum pageNum;
	LDB(pageHandle.GetPageNum(pageNum));
	IndexInfo *indexInfo = (IndexInfo*) pageData;
	indexInfo->attrType = attrType;
	indexInfo->attrLength = attrLength;
	indexInfo->rootPageNum = 1;
	LDB(fileHandle.MarkDirty(pageNum));
	LDB(fileHandle.ForcePages(pageNum));
	LDB(fileHandle.UnpinPage(pageNum));
	
	// allocate page 1 : LeafNode
	LDB(fileHandle.AllocatePage(pageHandle));
	LDB(pageHandle.GetPageNum(pageNum));
	LDB(pageHandle.GetData(pageData));
	NodePagePacket* nodePagePacket = (NodePagePacket*) pageData;
	nodePagePacket->nodeType = LEAF_NODE;
	(nodePagePacket->leafNode).init();
	LDB(fileHandle.MarkDirty(pageNum));
	LDB(fileHandle.ForcePages(pageNum));
	LDB(fileHandle.UnpinPage(pageNum));
	
	LDB(pfm->CloseFile(fileHandle));
	delete[] file;
	return 0;
}
	
RC IX_Manager::DestroyIndex(const char* filename, int indexNo) {
	// gen filename
	RC rc;
	char *file = GenFileName(filename, indexNo);
	if ((rc = pfm->DestroyFile(file)) != 0)
		return rc;
	delete[] file;
	return 0;
}


RC IX_Manager::OpenIndex(const char* filename, int indexNo, IX_IndexHandle &indexHandle) {
	RC rc;
	if ((rc = indexHandle.init(GenFileName(filename, indexNo), this->pfm)) != 0)
		return rc;
	return 0;
}

RC IX_Manager::CloseIndex(IX_IndexHandle &indexHandle) {
	RC rc;
	if ((rc = indexHandle.CloseIndex()) != 0)
		return rc;
	return 0;
}
	