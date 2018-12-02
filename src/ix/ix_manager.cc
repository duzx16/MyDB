#include "ix.h"
#include "ix_internal.h"
#include <cstring>
#include <cstdio>

IX_Manager::IX_Manager(PF_Manager &pfm) {
	this->pfm = &pfm;
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
	char *file = GenFileName(filename, indexNo);
	// open file with PF_Manager
	pfm->CreateFile(file);
	PF_FileHandle fileHandle;
	pfm->OpenFile(file, fileHandle);
	PF_PageHandle pageHandle;
	fileHandle.AllocatePage(pageHandle);
	char *pageData;
	pageHandle.GetData(pageData);
	PageNum pageNum;
	pageHandle.GetPageNum(pageNum);
	IndexInfo *indexInfo = (IndexInfo*) pageData;
	indexInfo->attrType = attrType;
	indexInfo->attrLength = attrLength;
	indexInfo->rootPageNum = 1;
	fileHandle.ForcePages(pageNum);
	fileHandle.UnpinPage(pageNum);
	
	fileHandle.AllocatePage(pageHandle);
	pageHandle.GetPageNum(pageNum);
	pageHandle.GetData(pageData);
	NodePagePacket* nodePagePacket = (NodePagePacket*) pageData;
	nodePagePacket->nodeType = 1;
	(nodePagePacket->leafNode).init();
	fileHandle.ForcePages(pageNum);
	fileHandle.UnpinPage(pageNum);
	
	pfm->CloseFile(fileHandle);
	delete[] file;
}
	
RC IX_Manager::DestroyIndex(const char* filename, int indexNo) {
	// gen filename
	char *file = GenFileName(filename, indexNo);
	pfm->DestroyFile(file);
	delete[] file;
	return 0;
}

RC IX_Manager::OpenIndex(const char* filename, int indexNo, IX_IndexHandle &indexHandle) {
	indexHandle.init(GenFileName(filename, indexNo), this->pfm);
}

RC IX_Manager::CloseIndex(IX_IndexHandle &indexHandle) {
	indexHandle.CloseIndex();
}
		
	