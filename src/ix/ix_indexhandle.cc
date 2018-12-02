#include "ix.h"
#include <cmath>

IX_IndexHandle::IX_IndexHandle() {}

IX_IndexHandle::~IX_IndexHandle() {}

void IX_IndexHandle::CloseIndex() {
	pfm->CloseFile(fileHandle);
}

RC IX_IndexHandle::InsertEntry(void *pData, const RID &rid) {
	PageNum ridPageNum = InsertEntryFromPage(pData, indexInfo->rootPageNum, -1, -1);
	RC rc = insertIntoRIDPage(rid, ridPageNum);
	ForcePages();
	return rc;
}

PageNum IX_IndexHandle::InsertEntryFromPage(void *pData, PageNum &pageNum, PageNum fatherPage, int nodePos) {
	PF_PageHandle pageHandle, fatherPageHandle;
	if (pageNum == -1) {// add new leaf
		fileHandle.AllocatePage(pageHandle);
		pageHandle.GetPageNum(pageNum);
	}
	char *pageData, *fatherPageData;
	fileHandle.GetThisPage(pageNum, pageHandle);
	pageHandle.GetData(pageData);
	if (fatherPage != -1) {
		fileHandle.GetThisPage(fatherPage, fatherPageHandle);
		fatherPageHandle.GetData(fatherPageData);
	}
	
	NodePagePacket* nodePagePacket = (NodePagePacket*)pageData;
	if (nodePagePacket->nodeType == 1) {// leaf node
		LeafNode *leafNode = &(nodePagePacket->leafNode);
		for (int i = 0; i < leafNode->size; ++i)
			if (cmp(pData, (leafNode->data[i]).pdata) == 0)
				return (leafNode->data[i]).ridPageNum;
		// not found, insert
		int pos;
		if (leafNode->size == 0)
			pos = 0;
		else {
			for (int i = 0; i <= leafNode->size; ++i) {
				if (cmp(pData, (leafNode->data[i]).pdata) < 0) {
					pos = i;
					break;
				}
				if (i == leafNode->size)
					pos = leafNode->size;
			}
		}
		
		PageNum newRIDPageNum;
		PF_PageHandle newRIDPageHandle;
		fileHandle.AllocatePage(newRIDPageHandle);
		newRIDPageHandle.GetPageNum(newRIDPageNum);
		fileHandle.MarkDirty(newRIDPageNum);
		leafNode->insertDataIntoPos(pData, newRIDPageNum, pos);
		fileHandle.MarkDirty(pageNum);
		// up overflow, split
		if (leafNode->size > NODE_KEYS) {
			PageNum splitNodePageNum;
			PF_PageHandle splitNodePageHandle;
			fileHandle.AllocatePage(splitNodePageHandle);
			splitNodePageHandle.GetPageNum(splitNodePageNum);
			fileHandle.MarkDirty(splitNodePageNum);
			char *splitNodePageData;
			splitNodePageHandle.GetData(splitNodePageData);
			((NodePagePacket*)splitNodePageData)->nodeType = 1;
			LeafNode* splitNode = &(((NodePagePacket*)splitNodePageData)->leafNode);
			splitNode->init();
			leafNode->split(splitNode, splitNodePageNum, pageNum);
			if (fatherPage == -1) {
				fileHandle.AllocatePage(fatherPageHandle);
				fatherPageHandle.GetPageNum(fatherPage);
				fatherPageHandle.GetData(fatherPageData);
				fileHandle.MarkDirty(fatherPage);
				((NodePagePacket*)fatherPageData)->nodeType = 0;
				InternalNode *fatherNode = &(((NodePagePacket*)fatherPageData)->internalNode);
				fatherNode->init();
				fatherNode->son[0] = pageNum;
				fatherNode->InsertKeyAfterPos((splitNode->data[0]).pdata, splitNodePageNum, 0);
				indexInfo->rootPageNum = fatherPage;
				fileHandle.MarkDirty(0);
			}
			else {
				InternalNode *fatherNode = &(((NodePagePacket*)fatherPageData)->internalNode);
				fatherNode->InsertKeyAfterPos((splitNode->data[0]).pdata, splitNodePageNum, nodePos);
			}
		}
		return newRIDPageNum;	
			
	}
	else {// internal node
		InternalNode *internalNode = &(nodePagePacket->internalNode);
		fileHandle.MarkDirty(pageNum);
		int result;
		if (cmp(pData, internalNode->pData[1]) < 0)
			result = InsertEntryFromPage(pData, internalNode->son[0], pageNum, 0);
		for (int i = 2; i <= internalNode->keyCount + 1; ++i) {
			if (cmp(pData, internalNode->pData[i]) < 0) {
				result = InsertEntryFromPage(pData, internalNode->son[i - 1], pageNum, i - 1);
				break;
			}
			if (i == internalNode->keyCount + 1)
				result = InsertEntryFromPage(pData, internalNode->son[internalNode->keyCount], pageNum, internalNode->keyCount);
		}
		if (internalNode->keyCount > NODE_KEYS) {
			PageNum splitNodePageNum;
			PF_PageHandle splitNodePageHandle;
			fileHandle.AllocatePage(splitNodePageHandle);
			splitNodePageHandle.GetPageNum(splitNodePageNum);
			fileHandle.MarkDirty(splitNodePageNum);
			char *splitNodePageData;
			splitNodePageHandle.GetData(splitNodePageData);
			((NodePagePacket*)splitNodePageData)->nodeType = 0;
			InternalNode* splitNode = &(((NodePagePacket*)splitNodePageData)->internalNode);
			splitNode->init();
			internalNode->Split(splitNode);
			if (fatherPage == -1) {
				fileHandle.AllocatePage(fatherPageHandle);
				fatherPageHandle.GetPageNum(fatherPage);
				fatherPageHandle.GetData(fatherPageData);
				fileHandle.MarkDirty(fatherPage);
				((NodePagePacket*)fatherPageData)->nodeType = 0;
				InternalNode *fatherNode = &(((NodePagePacket*)fatherPageData)->internalNode);
				fatherNode->init();
				fatherNode->son[0] = pageNum;
				fatherNode->InsertKeyAfterPos(splitNode->pData[0], splitNodePageNum, 0);
				indexInfo->rootPageNum = fatherPage;
				fileHandle.MarkDirty(0);
			}
			else {
				InternalNode *fatherNode = &(((NodePagePacket*)fatherPageData)->internalNode);
				fatherNode->InsertKeyAfterPos(splitNode->pData[0], splitNodePageNum, nodePos);
			}
		}
		return result;
	}
}
RC IX_IndexHandle::DeleteEntry(void *pData, const RID &rid) {
	PageNum ridPageNum = DeleteEntryFromPage(pData, indexInfo->rootPageNum, -1, -1);
	RC rc = deleteFromRIDPage(rid, ridPageNum);
	ForcePages();
	return rc;
}

PageNum IX_IndexHandle::DeleteEntryFromPage(void *pData, PageNum& pageNum, PageNum fatherPage, int nodePos) {}

RC IX_IndexHandle::ForcePages() {
	fileHandle.ForcePages();
}

void IX_IndexHandle::init(const char* indexFileName, PF_Manager *_pfm) {
	pfm = _pfm;
	pfm->OpenFile(indexFileName, fileHandle);
	PF_PageHandle pageHandle;
	fileHandle.GetThisPage(0, pageHandle);
	char *pageData;
	pageHandle.GetData(pageData);
	indexInfo = (IndexInfo*)pageData;
	//fileHandle.UnpinPage(0);
}

int IX_IndexHandle::cmp(void *a, void *b) {
	if (indexInfo->attrType == AttrType::INT) {
		int _a = *(int*)a, _b = *(int*)b;
		return _a - _b;
	}
	if (indexInfo->attrType == AttrType::FLOAT) {
		float _a = *(float*)a, _b = *(float*)b;
		float d = _a - _b;
		if (d >= -1e-4 && d <= 1e-4)
			return 0;
		return (d < 0) ? -1 : 1;
	}
	if (indexInfo->attrType == AttrType::STRING) {
		char *_a = (char*)a, *_b = (char*)b;
		for (int i = 0; i < indexInfo->attrLength; ++i)
			if (_a[i] != _b[i])
				return _a[i] - _b[i];
		return 0;
	}
	return 0;
}

RC IX_IndexHandle::insertIntoRIDPage(const RID rid, const PageNum pageNum) {
	PF_PageHandle pageHandle;
	fileHandle.GetThisPage(pageNum, pageHandle);
	char *pageData;
	pageHandle.GetData(pageData);
	RIDPagePacket* ridPagePacket = (RIDPagePacket*) pageData;
	ridPagePacket->insertRID(rid);
	fileHandle.MarkDirty(pageNum);
	fileHandle.UnpinPage(pageNum);
}

RC IX_IndexHandle::deleteFromRIDPage(const RID rid, const PageNum pageNum) {
	PF_PageHandle pageHandle;
	fileHandle.GetThisPage(pageNum, pageHandle);
	char *pageData;
	pageHandle.GetData(pageData);
	RIDPagePacket* ridPagePacket = (RIDPagePacket*) pageData;
	ridPagePacket->deleteRID(rid);
	fileHandle.MarkDirty(pageNum);
	fileHandle.UnpinPage(pageNum);
}

PageNum IX_IndexHandle::FindLeafPageFromPage(void *pData, PageNum pageNum) {
	PageNum result;
	PF_PageHandle pageHandle;
	fileHandle.GetThisPage(pageNum, pageHandle);
	char *pageData;
	pageHandle.GetData(pageData);
	NodePagePacket* nodePagePacket = (NodePagePacket*) pageData;
	if (nodePagePacket->nodeType == 1) {// leafNode
		result = pageNum;
	}
	else {// internalNode
		InternalNode *internalNode = &(nodePagePacket->internalNode);
		for (int i = 1; i <= internalNode->keyCount + 1; ++i) {
			if (cmp(pData, internalNode->pData[i]) < 0) {
				result = FindLeafPageFromPage(pData, internalNode->son[i - 1];
				break;
			}
			if (i == internalNode->keyCount + 1) {
				result = FindLeafPageFromPage(pData, internalNode->son[internalNode->keyCount]);
			}
		}
	}
	fileHandle.UnpinPage(pageNum);
	return result;
}

LeafNode IX_IndexHandle::FindLeafNode(void *pData) {
	PageNum leafPage = FindLeafPageFromPage(pData, indexInfo->rootPageNum);
	PF_PageHandle pageHandle;
	fileHandle.GetThisPage(leafPage, pageHandle);
	char* pageData;
	pageHandle.GetData(pageData);
	fileHandle.UnpinPage(leafPage);
	return ((NodePagePacket*)pageData)->leafNode;
}