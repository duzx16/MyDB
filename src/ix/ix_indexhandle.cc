#include "ix.h"
#include <cmath>
#include <cassert>
#include <cstdio>
IX_IndexHandle::IX_IndexHandle() {
	pinnedPageNum = 0;
}

IX_IndexHandle::~IX_IndexHandle() {}

void IX_IndexHandle::CloseIndex() {
	LDB(fileHandle.UnpinPage(0));
	LDB(pfm->CloseFile(fileHandle));
}

RC IX_IndexHandle::InsertEntry(void *pData, const RID &rid) {
	PageNum ridPageNum = InsertEntryFromPage(pData, indexInfo->rootPageNum, -1, -1);
	RC rc = insertIntoRIDPage(rid, ridPageNum);
	ForcePages();
	unpinAllPages();
	return rc;
}

PageNum IX_IndexHandle::InsertEntryFromPage(void *pData, PageNum &pageNum, PageNum fatherPage, int nodePos) {
	PF_PageHandle pageHandle, fatherPageHandle;
	char *pageData, *fatherPageData;
	if (pageNum == -1) {
		pageNum = allocateNewPage(pageHandle);
	}
	else {
		getExistedPage(pageNum, pageHandle);
	}
	LDB(pageHandle.GetData(pageData));
	
	NodePagePacket* nodePagePacket = (NodePagePacket*)pageData;
	if (nodePagePacket->nodeType == LEAF_NODE) {
		// leaf node
		LeafNode *leafNode = &(nodePagePacket->leafNode);
		for (int i = 0; i < leafNode->size; ++i)
			if (cmp(pData, (leafNode->data[i]).pdata) == 0) {
				return (leafNode->data[i]).ridPageNum;
			}
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
		
		PF_PageHandle newRIDPageHandle;
		PageNum newRIDPageNum = allocateNewPage(newRIDPageHandle);
		leafNode->insertDataIntoPos(pData, newRIDPageNum, pos);
		LDB(fileHandle.MarkDirty(pageNum));
		LDB(fileHandle.ForcePages(pageNum));
		// overflow, split
		if (leafNode->size > 2 * D) {
			PF_PageHandle splitNodePageHandle;
			PageNum splitNodePageNum = allocateNewPage(splitNodePageHandle);
			LDB(fileHandle.MarkDirty(splitNodePageNum));
			char *splitNodePageData;
			LDB(splitNodePageHandle.GetData(splitNodePageData));
			((NodePagePacket*)splitNodePageData)->nodeType = LEAF_NODE;
			LeafNode* splitNode = &(((NodePagePacket*)splitNodePageData)->leafNode);
			splitNode->init();
			leafNode->split(splitNode, splitNodePageNum, pageNum);
			LDB(fileHandle.ForcePages(pageNum));
			LDB(fileHandle.ForcePages(splitNodePageNum));
			// change fatherPage
			if (fatherPage == -1) {
				fatherPage = allocateNewPage(fatherPageHandle);
				LDB(fatherPageHandle.GetData(fatherPageData));
				LDB(fileHandle.MarkDirty(fatherPage));
				((NodePagePacket*)fatherPageData)->nodeType = INTERNAL_NODE;
				InternalNode *fatherNode = &(((NodePagePacket*)fatherPageData)->internalNode);
				fatherNode->init();
				fatherNode->son[0] = pageNum;
				fatherNode->InsertKeyAfterPos((splitNode->data[0]).pdata, splitNodePageNum, 0);
				indexInfo->rootPageNum = fatherPage;
				LDB(fileHandle.MarkDirty(0));
			}
			else {
				getExistedPage(fatherPage, fatherPageHandle);
				LDB(fatherPageHandle.GetData(fatherPageData));
				InternalNode *fatherNode = &(((NodePagePacket*)fatherPageData)->internalNode);
				fatherNode->InsertKeyAfterPos((splitNode->data[0]).pdata, splitNodePageNum, nodePos);
				LDB(fileHandle.MarkDirty(fatherPage));
			}
			LDB(fileHandle.ForcePages(fatherPage));
		}
		return newRIDPageNum;	
			
	}
	else {
		// internal node
		InternalNode *internalNode = &(nodePagePacket->internalNode);
		LDB(fileHandle.MarkDirty(pageNum));
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
		// reload after rec
		getExistedPage(pageNum, pageHandle);
		LDB(pageHandle.GetData(pageData));
		NodePagePacket* nodePagePacket = (NodePagePacket*)pageData;
		internalNode = &(nodePagePacket->internalNode);
		
		if (internalNode->keyCount == D * 2) {
			PF_PageHandle splitNodePageHandle;
			PageNum splitNodePageNum = allocateNewPage(splitNodePageHandle);
			LDB(fileHandle.MarkDirty(splitNodePageNum));
			char *splitNodePageData;
			LDB(splitNodePageHandle.GetData(splitNodePageData));
			((NodePagePacket*)splitNodePageData)->nodeType = INTERNAL_NODE;
			InternalNode* splitNode = &(((NodePagePacket*)splitNodePageData)->internalNode);
			splitNode->init();
			internalNode->Split(splitNode);
			
			LDB(fileHandle.ForcePages(pageNum));
			LDB(fileHandle.ForcePages(splitNodePageNum));
			// modify father page
			if (fatherPage == -1) {
				fatherPage = allocateNewPage(fatherPageHandle);
				fatherPageHandle.GetData(fatherPageData);
				fileHandle.MarkDirty(fatherPage);
				((NodePagePacket*)fatherPageData)->nodeType = 0;
				InternalNode *fatherNode = &(((NodePagePacket*)fatherPageData)->internalNode);
				fatherNode->init();
				fatherNode->son[0] = pageNum;
				fatherNode->InsertKeyAfterPos(splitNode->pData[0], splitNodePageNum, 0);
				indexInfo->rootPageNum = fatherPage;
				LDB(fileHandle.MarkDirty(0));
			}
			else {
				getExistedPage(fatherPage, fatherPageHandle);
				LDB(fatherPageHandle.GetData(fatherPageData));
				InternalNode *fatherNode = &(((NodePagePacket*)fatherPageData)->internalNode);
				fatherNode->InsertKeyAfterPos(splitNode->pData[0], splitNodePageNum, nodePos);
				LDB(fileHandle.MarkDirty(fatherPage));
			}
			LDB(fileHandle.ForcePages(fatherPage));
		}
		return result;
	}
}

RC IX_IndexHandle::DeleteEntry(void *pData, const RID &rid) {
	void *_;
	RC rc = DeleteEntryFromPage(pData, indexInfo->rootPageNum, -1, rid, -1);
	ForcePages();
	unpinAllPages();
	return rc;
}

int IX_IndexHandle::getRIDPageSize(const PageNum pageNum) {
	char *pageData;
	getPageData(pageNum, pageData);
	return ((RIDPagePacket*)pageData)->size;
}

int IX_IndexHandle::getLeafNodeSize(const PageNum pageNum) {
	char *pageData;
	getPageData(pageNum, pageData);
	return (((NodePagePacket*)pageData)->leafNode).size;
}

void IX_IndexHandle::getPageData(const PageNum pageNum, char*& pageData) {
	PF_PageHandle pageHandle;
	assert(pageNum != -1);
	getExistedPage(pageNum, pageHandle);
	LDB(pageHandle.GetData(pageData));
}

void IX_IndexHandle::getLeafNode(const PageNum pageNum, LeafNode*& leafNode) {
	char *pageData;
	getPageData(pageNum, pageData);
	leafNode = &(((NodePagePacket*)pageData)->leafNode);
}

void IX_IndexHandle::getInternalNode(const PageNum pageNum, InternalNode*& internalNode) {
	char *pageData;
	getPageData(pageNum, pageData);
	internalNode = &(((NodePagePacket*)pageData)->internalNode);
}

RC IX_IndexHandle::DeleteEntryFromPage(void *pData, PageNum& pageNum, PageNum fatherPageNum, const RID &rid, int thisPos) {
	char *pageData, *fatherPageData;
	getPageData(pageNum, pageData);
	NodePagePacket* nodePagePacket = (NodePagePacket*)pageData;
	if (nodePagePacket->nodeType == 0) {
		// internal node, find correct son pos
		InternalNode *internalNode = &(nodePagePacket->internalNode);
		for (int i = 1; i <= internalNode->keyCount + 1; ++i) {
			if (i == internalNode->keyCount + 1 || cmp(pData, internalNode->pData[i]) < 0) {
				// recursion
				DeleteEntryFromPage(pData, internalNode->son[i - 1], pageNum, rid, i - 1);
				break;
			}
		}
		// under-flow detection
		if (internalNode->keyCount + 1 < D && pageNum != indexInfo->rootPageNum) {
			getPageData(fatherPageNum, fatherPageData);
			InternalNode *pa = &(((NodePagePacket*)fatherPageData)->internalNode);
			
		}
		
	}
	else {
		// leaf node
		LeafNode *leafNode = &(nodePagePacket->leafNode);
		for (int i = 0; i < leafNode->size; ++i) {
			if (cmp(pData, (leafNode->data[i]).pdata) == 0) {
				// find index val, delete rid
				deleteFromRIDPage(rid, (leafNode->data[i]).ridPageNum);
				if (getRIDPageSize((leafNode->data[i]).ridPageNum) == 0) {
					// reallocate page & delete index val in the tree
					fileHandle.DisposePage((leafNode->data[i]).ridPageNum);
					leafNode->deleteDataAtPos(i);
					fileHandle.MarkDirty(pageNum);
					// check under-flow condition
					if (leafNode->size < D && pageNum != indexInfo->rootPageNum) {
						if (leafNode->leftPage != -1 && getLeafNodeSize(leafNode->leftPage) > D) {
							// redistribute from left page
							LeafNode* leftLeafNode;
							getLeafNode(leafNode->leftPage, leftLeafNode);
							LeafData data = leftLeafNode->data[leftLeafNode->size - 1];
							leftLeafNode->deleteDataAtPos(leftLeafNode->size - 1);
							fileHandle.MarkDirty(leafNode->leftPage);
							leafNode->insertDataIntoPos(data.pdata, data.ridPageNum, leafNode->size);
							fileHandle.MarkDirty(pageNum);
							InternalNode *pa;
							getInternalNode(fatherPageNum, pa);
							if (thisPos > 0) {
								
							}
						}
						else if (leafNode->rightPage != -1 && getLeafNodeSize(leafNode->rightPage) > D) {
							// redistribute from right page
						}
						else if (leafNode->rightPage != -1) {
							// merge with left page
						}
						else {
							// merge with right page
						}
					}
				}
			}
		}
	}
}

RC IX_IndexHandle::ForcePages() {
	return fileHandle.ForcePages();
}

void IX_IndexHandle::init(const char* indexFileName, PF_Manager *_pfm) {
	pfm = _pfm;
	LDB(pfm->OpenFile(indexFileName, fileHandle));
	PF_PageHandle pageHandle;
	LDB(fileHandle.GetThisPage(0, pageHandle));
	char *pageData;
	LDB(pageHandle.GetData(pageData));
	indexInfo = (IndexInfo*)pageData;
	//fileHandle.UnpinPage(0);
}

RC IX_IndexHandle::insertIntoRIDPage(const RID rid, const PageNum pageNum) {
	PF_PageHandle pageHandle;
	//LDB(fileHandle.GetThisPage(pageNum, pageHandle));
	getExistedPage(pageNum, pageHandle);
	char *pageData;
	LDB(pageHandle.GetData(pageData));
	RIDPagePacket* ridPagePacket = (RIDPagePacket*) pageData;
	int rc = ridPagePacket->insertRID(rid);
	LDB(fileHandle.MarkDirty(pageNum));
	return rc;
}

RC IX_IndexHandle::deleteFromRIDPage(const RID rid, const PageNum pageNum) {
	PF_PageHandle pageHandle;
	//LDB(fileHandle.GetThisPage(pageNum, pageHandle));
	getExistedPage(pageNum, pageHandle);
	char *pageData;
	LDB(pageHandle.GetData(pageData));
	RIDPagePacket* ridPagePacket = (RIDPagePacket*) pageData;
	int rc = ridPagePacket->deleteRID(rid);
	LDB(fileHandle.MarkDirty(pageNum));
	return rc;
}

PageNum IX_IndexHandle::FindLeafPageFromPage(void *pData, PageNum pageNum) {
	PageNum result;
	PF_PageHandle pageHandle;
	LDB(fileHandle.GetThisPage(pageNum, pageHandle));
	char *pageData;
	LDB(pageHandle.GetData(pageData));
	NodePagePacket* nodePagePacket = (NodePagePacket*) pageData;
	if (nodePagePacket->nodeType == 1) {// leafNode
		result = pageNum;
	}
	else {// internalNode
		InternalNode *internalNode = &(nodePagePacket->internalNode);
		for (int i = 1; i <= internalNode->keyCount + 1; ++i) {
			if (cmp(pData, internalNode->pData[i]) < 0) {
				result = FindLeafPageFromPage(pData, internalNode->son[i - 1]);
				break;
			}
			if (i == internalNode->keyCount + 1) {
				result = FindLeafPageFromPage(pData, internalNode->son[internalNode->keyCount]);
			}
		}
	}
	LDB(fileHandle.UnpinPage(pageNum));
	return result;
}

LeafNode IX_IndexHandle::FindLeafNode(void *pData) {
	PageNum leafPage = FindLeafPageFromPage(pData, indexInfo->rootPageNum);
	PF_PageHandle pageHandle;
	LDB(fileHandle.GetThisPage(leafPage, pageHandle));
	char* pageData;
	LDB(pageHandle.GetData(pageData));
	LDB(fileHandle.UnpinPage(leafPage));
	return ((NodePagePacket*)pageData)->leafNode;
}

void IX_IndexHandle::PrintFullLinkList() {
	PF_PageHandle pageHandle;
	char *pageData;
	getExistedPage(indexInfo->rootPageNum, pageHandle);
	LeafNode *leafNode;
	while (true) {
		LDB(pageHandle.GetData(pageData));
		NodePagePacket* nodePagePacket = (NodePagePacket*) pageData;
		if (nodePagePacket->nodeType == INTERNAL_NODE)
			getExistedPage((nodePagePacket->internalNode).son[0], pageHandle);
		else {
			leafNode = &(nodePagePacket->leafNode); 
			break;
		}
	}
	while (true) {
		for (int i = 0; i < leafNode->size; ++i)
			printf("data = %d, RIDPage = %d\n", *((int*)((leafNode->data[i]).pdata)), (leafNode->data[i]).ridPageNum);
		if (leafNode->rightPage == -1)
			break;
		getExistedPage(leafNode->rightPage, pageHandle);
		LDB(pageHandle.GetData(pageData));
		leafNode = &(((NodePagePacket*) pageData)->leafNode);
	}
	unpinAllPages();
}

void IX_IndexHandle::GetGeqRIDPos(void *pData, RIDPositionInfo &ridPositionInfo, bool returnFirstRID) {
	PF_PageHandle pageHandle;
	char *pageData;
	//LDB(fileHandle.GetThisPage(indexInfo->rootPageNum, pageHandle));
	getExistedPage(indexInfo->rootPageNum, pageHandle);
	LeafNode *leafNode;
	PageNum curPage;
	while (true) {
		LDB(pageHandle.GetData(pageData));
		NodePagePacket* nodePagePacket = (NodePagePacket*) pageData;
		if (nodePagePacket->nodeType == 0)
			//fileHandle.GetThisPage((nodePagePacket->internalNode).son[0], pageHandle);
			getExistedPage((nodePagePacket->internalNode).son[0], pageHandle);
		else {
			LDB(pageHandle.GetPageNum(curPage));
			leafNode = &(nodePagePacket->leafNode); 
			break;
		}
	}
	bool find = false;
	while (true) {
		for (int i = 0; i < leafNode->size; ++i) {
			if (cmp(pData, (leafNode->data[i]).pdata) <= 0 || returnFirstRID) {
				ridPositionInfo.leafNode = *leafNode;
				ridPositionInfo.posInLeaf = i;
				//fileHandle.GetThisPage((leafNode->data[i]).ridPageNum, pageHandle);
				getExistedPage((leafNode->data[i]).ridPageNum, pageHandle);
				LDB(pageHandle.GetData(pageData));
				ridPositionInfo.ridPagePacket = *((RIDPagePacket*)pageData);
				ridPositionInfo.ridPagePos = 0;
				find = true;
				break;
			}
		}
		if (find)
			break;
		if (leafNode->rightPage == -1) {
			ridPositionInfo.ridPagePos = -1;
			break;
		}
		else {
			//fileHandle.GetThisPage(leafNode->rightPage, pageHandle);
			getExistedPage(leafNode->rightPage, pageHandle);
			pageHandle.GetData(pageData);
			leafNode = &(((NodePagePacket*)pageData)->leafNode);
		}
	}
	unpinAllPages();
}

void IX_IndexHandle::GetNextRIDPositionInfo(RIDPositionInfo &ridPositionInfo, int dir, bool EQ_OP) {
	char *pageData;
	PF_PageHandle pageHandle;
	if (ridPositionInfo.ridPagePos < ridPositionInfo.ridPagePacket.size - 1)
		++ridPositionInfo.ridPagePos;
	else {
		if (EQ_OP) {
			ridPositionInfo.ridPagePos = -1;
		}
		else if (dir == 1) {// right
			if (ridPositionInfo.posInLeaf < ridPositionInfo.leafNode.size - 1) {
				ridPositionInfo.ridPagePos = 0;
				//fileHandle.GetThisPage(ridPositionInfo.leafNode.data[++ridPositionInfo.posInLeaf].ridPageNum, pageHandle);
				getExistedPage(ridPositionInfo.leafNode.data[++ridPositionInfo.posInLeaf].ridPageNum, pageHandle);
				LDB(pageHandle.GetData(pageData));
				ridPositionInfo.ridPagePacket = *((RIDPagePacket*)pageData);
			}
			else {
				if (ridPositionInfo.leafNode.rightPage == -1) {
					ridPositionInfo.ridPagePos = -1;
				}
				else {
					//fileHandle.GetThisPage(ridPositionInfo.leafNode.rightPage, pageHandle);
					getExistedPage(ridPositionInfo.leafNode.rightPage, pageHandle);
					LDB(pageHandle.GetData(pageData));
					ridPositionInfo.leafNode = ((NodePagePacket*)pageData)->leafNode;
					ridPositionInfo.posInLeaf = 0;
					//fileHandle.GetThisPage(ridPositionInfo.leafNode.data[0].ridPageNum, pageHandle);
					getExistedPage(ridPositionInfo.leafNode.data[0].ridPageNum, pageHandle);
					LDB(pageHandle.GetData(pageData));
					ridPositionInfo.ridPagePacket = *((RIDPagePacket*)pageData);
					ridPositionInfo.ridPagePos = 0;
				}
			}
		}
		else {// left
			if (ridPositionInfo.posInLeaf > 0) {
				ridPositionInfo.ridPagePos = 0;
				//fileHandle.GetThisPage(ridPositionInfo.leafNode.data[--ridPositionInfo.posInleaf].ridPageNum, pageHandle);
				getExistedPage(ridPositionInfo.leafNode.data[--ridPositionInfo.posInLeaf].ridPageNum, pageHandle);
				LDB(pageHandle.GetData(pageData));
				ridPositionInfo.ridPagePacket = *((RIDPagePacket*)pageData);
			}
			else {
				if (ridPositionInfo.leafNode.leftPage == -1) {
					ridPositionInfo.ridPagePos = -1;
				}
				else {
					//fileHandle.GetThisPage(ridPositionInfo.leafNode.leftPage, pageHandle);
					getExistedPage(ridPositionInfo.leafNode.leftPage, pageHandle);
					LDB(pageHandle.GetData(pageData));
					ridPositionInfo.leafNode = ((NodePagePacket*)pageData)->leafNode;
					ridPositionInfo.posInLeaf = 0;
					//fileHandle.GetThisPage(ridPositionInfo.leafNode.data[0].ridPageNum, pageHandle);
					getExistedPage(ridPositionInfo.leafNode.data[0].ridPageNum, pageHandle);
					LDB(pageHandle.GetData(pageData));
					ridPositionInfo.ridPagePacket = *((RIDPagePacket*)pageData);
					ridPositionInfo.ridPagePos = 0;
				}
			}
		}
	}
	unpinAllPages();
}

PageNum IX_IndexHandle::allocateNewPage(PF_PageHandle &pageHandle) {
	LDB(fileHandle.AllocatePage(pageHandle));
	PageNum pageNum;
	LDB(pageHandle.GetPageNum(pageNum));
	addPinnedPage(pageNum);
	return pageNum;
}

void IX_IndexHandle::getExistedPage(PageNum pageNum, PF_PageHandle &pageHandle) {
	LDB(fileHandle.GetThisPage(pageNum, pageHandle));
	addPinnedPage(pageNum);
}
void IX_IndexHandle::addPinnedPage(PageNum pageNum) {
	pinnedPageList[pinnedPageNum++] = pageNum;
}

void IX_IndexHandle::unpinAllPages() {
	for (int i = 0; i < pinnedPageNum; ++i) {
		/*
		bool existed = false;
		for (int j = 0; j < i; ++j) {
			if (pinnedPageList[j] == pinnedPageList[i]) {
				//existed before
				existed = true;
				break;
			}
		}
		if (!existed) {
			LDB(fileHandle.UnpinPage(pinnedPageList[i]));
			printf("Unpinpage %d\n", pinnedPageList[i]);
		}
		*/
		LDB(fileHandle.UnpinPage(pinnedPageList[i]));
	}
	pinnedPageNum = 0;
}

int IX_IndexHandle::cmp(void *a, void *b) {
	if (a == NULL || b == NULL)
		return 0;
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