#include "ix.h"
#include <cmath>
#include <cassert>
#include <cstdio>
#include "../rm/RM_Record.h"

IX_IndexHandle::IX_IndexHandle() {
	pinnedPageNum = 0;
	disposedPageNum = 0;
}

IX_IndexHandle::~IX_IndexHandle() {
    pfm->CloseFile(fileHandle);
}

RC IX_IndexHandle::CloseIndex() {
	LDB(fileHandle.UnpinPage(0));
	LDB(pfm->CloseFile(fileHandle));
	return 0;
}

RC IX_IndexHandle::InsertEntry(const RID &rid) {
	printf("page = %d, slot = %d\n", rid.getPageNum(), rid.getSlotNum());
	RC rc;
	PageNum ridPageNum = InsertEntryFromPage(rid, indexInfo->rootPageNum, -1, -1);
	if ((rc = insertIntoRIDPage(rid, ridPageNum)) != 0)
		return rc;
	ForcePages();
	unpinAllPages();
	return 0;
}

PageNum IX_IndexHandle::InsertEntryFromPage(RID rid, PageNum &pageNum, PageNum fatherPage, int nodePos) {
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
			if (cmp(rid, (leafNode->data[i]).recRID) == 0) {
				return (leafNode->data[i]).ridPageNum;
			}
		// not found, insert
		int pos;
		if (leafNode->size == 0)
			pos = 0;
		else {
			for (int i = 0; i <= leafNode->size; ++i) {
				if (cmp(rid, (leafNode->data[i]).recRID) < 0) {
					pos = i;
					break;
				}
				if (i == leafNode->size)
					pos = leafNode->size;
			}
		}

		PageNum newRIDPageNum;
		PF_PageHandle newRIDPageHandle;
		LDB(fileHandle.AllocatePage(newRIDPageHandle));
		LDB(newRIDPageHandle.GetPageNum(newRIDPageNum));
		LDB(fileHandle.UnpinPage(newRIDPageNum));
		
		leafNode->insertDataIntoPos(rid, newRIDPageNum, pos);
		LDB(fileHandle.MarkDirty(pageNum));
		LDB(fileHandle.ForcePages(pageNum));
		// overflow, split
		if (leafNode->size > 2 * D) {
			PF_PageHandle splitNodePageHandle;
			PageNum splitNodePageNum = allocateNewPage(splitNodePageHandle);
			LDB(fileHandle.MarkDirty(splitNodePageNum));
			LDB(fileHandle.MarkDirty(pageNum));
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
				fatherNode->recRID[0] = (leafNode->data[0]).recRID;
				fatherNode->InsertKeyAfterPos((splitNode->data[0]).recRID, splitNodePageNum, 0);
				indexInfo->rootPageNum = fatherPage;
				LDB(fileHandle.MarkDirty(0));
				LDB(fileHandle.ForcePages(0));
			}
			else {
				getExistedPage(fatherPage, fatherPageHandle);
				LDB(fatherPageHandle.GetData(fatherPageData));
				InternalNode *fatherNode = &(((NodePagePacket*)fatherPageData)->internalNode);
				fatherNode->InsertKeyAfterPos((splitNode->data[0]).recRID, splitNodePageNum, nodePos);
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
		for (int i = 1; i <= internalNode->keyCount + 1; ++i) {
			if (i == internalNode->keyCount + 1 || cmp(rid, internalNode->recRID[i]) < 0) {
				result = InsertEntryFromPage(rid, internalNode->son[i - 1], pageNum, i - 1);
				break;
			}
		}
		// reload after rec
		/*
		getExistedPage(pageNum, pageHandle);
		LDB(pageHandle.GetData(pageData));
		NodePagePacket* nodePagePacket = (NodePagePacket*)pageData;
		internalNode = &(nodePagePacket->internalNode);
		*/

		if (internalNode->keyCount == D * 2) {
			PF_PageHandle splitNodePageHandle;
			PageNum splitNodePageNum = allocateNewPage(splitNodePageHandle);
			LDB(fileHandle.MarkDirty(splitNodePageNum));
			LDB(fileHandle.MarkDirty(pageNum));
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
				fatherNode->recRID[0] = internalNode->recRID[0];
				fatherNode->InsertKeyAfterPos(splitNode->recRID[0], splitNodePageNum, 0);
				indexInfo->rootPageNum = fatherPage;
				LDB(fileHandle.MarkDirty(0));
				LDB(fileHandle.ForcePages(0));
			}
			else {
				getExistedPage(fatherPage, fatherPageHandle);
				LDB(fatherPageHandle.GetData(fatherPageData));
				InternalNode *fatherNode = &(((NodePagePacket*)fatherPageData)->internalNode);
				fatherNode->InsertKeyAfterPos(splitNode->recRID[0], splitNodePageNum, nodePos);
				LDB(fileHandle.MarkDirty(fatherPage));
			}
			LDB(fileHandle.ForcePages(fatherPage));
		}
		return result;
	}
}

RC IX_IndexHandle::DeleteEntry(const RID &rid) {
	RC rc;
	if ((rc = DeleteEntryFromPage(rid, indexInfo->rootPageNum, -1, -1)) != 0)
		return rc;
	ForcePages();
	unpinAllPages();
	disposeAllPages();
	return 0;
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

RC IX_IndexHandle::DeleteEntryFromPage(RID rid, PageNum& pageNum, PageNum fatherPageNum, int thisPos) {
	RC rc;
	char *pageData, *fatherPageData;
	PF_PageHandle pageHandle, fatherPageHandle;
	getExistedPage(pageNum, pageHandle);
	LDB(pageHandle.GetData(pageData));
	//getPageData(pageNum, pageData);
	NodePagePacket* nodePagePacket = (NodePagePacket*)pageData;
	if (nodePagePacket->nodeType == 0) {
		// internal node, find correct son pos
		InternalNode *internalNode = &(nodePagePacket->internalNode);
		for (int i = 1; i <= internalNode->keyCount + 1; ++i) {
			if (i == internalNode->keyCount + 1 || cmp(rid, internalNode->recRID[i]) < 0) {
				// recursion
				if ((rc = DeleteEntryFromPage(rid, internalNode->son[i - 1], pageNum, i - 1)) != 0)
					return rc;
				break;
			}
		}
		if (internalNode->keyCount == -1) {
			addDisposedPage(pageNum);
			if (fatherPageNum != -1) {
				getExistedPage(fatherPageNum, fatherPageHandle);
				LDB(fatherPageHandle.GetData(fatherPageData));
				//getPageData(fatherPageNum, fatherPageData);
				InternalNode *fatherNode = &(((NodePagePacket*)fatherPageData)->internalNode);
				fatherNode->DeleteKeyAtPos(thisPos);
				LDB(fileHandle.MarkDirty(fatherPageNum));
				LDB(fileHandle.ForcePages(fatherPageNum));
			}
		}
		return 0;
	}
	else {
		// leaf node
		LeafNode *leafNode = &(nodePagePacket->leafNode);
		for (int i = 0; i < leafNode->size; ++i) {
			if (cmp(rid, (leafNode->data[i]).recRID) == 0) {
				// find index val, delete rid
				int lastRIDCount;
				if ((rc = deleteFromRIDPage(rid, (leafNode->data[i]).ridPageNum, lastRIDCount)) != 0)
					return rc;
				if (lastRIDCount == 0) {
					// reallocate page & delete index val in the tree
					addDisposedPage((leafNode->data[i]).ridPageNum);
					leafNode->deleteDataAtPos(i);
					LDB(fileHandle.MarkDirty(pageNum));
					LDB(fileHandle.ForcePages(pageNum));
				}
				if (leafNode->size == 0) {
					addDisposedPage(pageNum);
					if (fatherPageNum != -1) {
						//getPageData(fatherPageNum, fatherPageData);
						getExistedPage(fatherPageNum, fatherPageHandle);
						LDB(fatherPageHandle.GetData(fatherPageData));
						InternalNode *fatherNode = &(((NodePagePacket*)fatherPageData)->internalNode);
						fatherNode->DeleteKeyAtPos(thisPos);
						LDB(fileHandle.MarkDirty(fatherPageNum));
						LDB(fileHandle.ForcePages(fatherPageNum));
					}
				}
				return 0;
			}
		}
		return IX_ENTRY_DOES_NOT_EXIST;
	}
}

RC IX_IndexHandle::init(const char* indexFileName, PF_Manager *_pfm, RM_FileHandle *_rmFileHandle, int _attrOffset) {
	RC rc;
	pfm = _pfm;
	rmFileHandle = _rmFileHandle;
	attrOffset = _attrOffset;
	if ((rc = pfm->OpenFile(indexFileName, fileHandle)) != 0)
		return rc;
	PF_PageHandle pageHandle;
	LDB(fileHandle.GetThisPage(0, pageHandle));
	char *pageData;
	LDB(pageHandle.GetData(pageData));
	indexInfo = (IndexInfo*)pageData;
	return 0;
	//fileHandle.UnpinPage(0);
}

RC IX_IndexHandle::insertIntoRIDPage(const RID rid, const PageNum pageNum) {
	PF_PageHandle pageHandle;
	LDB(fileHandle.GetThisPage(pageNum, pageHandle));
	//getExistedPage(pageNum, pageHandle);
	char *pageData;
	LDB(pageHandle.GetData(pageData));
	RIDPagePacket* ridPagePacket = (RIDPagePacket*) pageData;
	int rc = ridPagePacket->insertRID(rid);
	LDB(fileHandle.MarkDirty(pageNum));
	LDB(fileHandle.ForcePages(pageNum));
	LDB(fileHandle.UnpinPage(pageNum));
	return rc;
}

RC IX_IndexHandle::deleteFromRIDPage(const RID rid, const PageNum pageNum, int &lastRIDCount) {
	RC rc;
	PF_PageHandle pageHandle;
	//LDB(fileHandle.GetThisPage(pageNum, pageHandle));
	getExistedPage(pageNum, pageHandle);
	char *pageData;
	LDB(pageHandle.GetData(pageData));
	RIDPagePacket* ridPagePacket = (RIDPagePacket*) pageData;
	if ((rc = ridPagePacket->deleteRID(rid)) != 0)
		return rc;
	lastRIDCount = ridPagePacket->size;
	LDB(fileHandle.MarkDirty(pageNum));
	LDB(fileHandle.ForcePages(pageNum));
	return 0;
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
			if (cmp(pData, getValueFromRecRID(internalNode->recRID[i])) < 0) {
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
/*
void IX_IndexHandle::PrintFullLinkList() {
	PF_PageHandle pageHandle;
	//getExistedPage(2, pageHandle);
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
		for (int i = 0; i < leafNode->size; ++i) {
			printf("data = %d, RIDPage = %ld\n", *((int*)((leafNode->data[i]).pdata)), (leafNode->data[i]).ridPageNum);
		}
		int rightPage = leafNode->rightPage;
		unpinAllPages();

		if (rightPage == -1)
			break;
		getExistedPage(rightPage, pageHandle);
		LDB(pageHandle.GetData(pageData));
		leafNode = &(((NodePagePacket*) pageData)->leafNode);
	}
	unpinAllPages();
}
*/
void IX_IndexHandle::GetGeqRIDPos(const void *pData, RIDPositionInfo &ridPositionInfo, bool returnFirstRID) {
	PF_PageHandle pageHandle;
	char *pageData;
	//getExistedPage(2, pageHandle);
	getExistedPage(indexInfo->rootPageNum, pageHandle);
	LeafNode *leafNode;
	while (true) {
		LDB(pageHandle.GetData(pageData));
		NodePagePacket* nodePagePacket = (NodePagePacket*) pageData;
		if (nodePagePacket->nodeType == INTERNAL_NODE) {
			InternalNode *internalNode = &(nodePagePacket->internalNode);
			int i;
			for (i = internalNode->keyCount; i >= 0; --i) {
				if (cmp(getValueFromRecRID(internalNode->recRID[i]), pData) < 0) {
					int son = internalNode->son[i];
					unpinAllPages();
					getExistedPage(son, pageHandle);
					break;
				}
			}
			if (i == -1) {
				int son = internalNode->son[0];
				unpinAllPages();
				getExistedPage(son, pageHandle);
			}
		}
		else {
			leafNode = &(nodePagePacket->leafNode);
			break;
		}
	}
	bool find = false;
	while (true) {
		for (int i = 0; i < leafNode->size; ++i) {
			//printf("data = %d, RIDPage = %d\n", *((int*)((leafNode->data[i]).pdata)), (leafNode->data[i]).ridPageNum);
			if (cmp(pData, getValueFromRecRID((leafNode->data[i]).recRID)) <= 0 || returnFirstRID) {
				ridPositionInfo.leafNode = *leafNode;
				ridPositionInfo.posInLeaf = i;
				ridPositionInfo.ridPagePos = 0;
				getExistedPage((leafNode->data[i]).ridPageNum, pageHandle);
				LDB(pageHandle.GetData(pageData));
				ridPositionInfo.ridPagePacket = *(RIDPagePacket*)pageData;
				ridPositionInfo.value = getValueFromRecRID((leafNode->data[i]).recRID);
				find = true;
				break;
			}
		}
		if (find)
			break;
		int rightPage = leafNode->rightPage;
		unpinAllPages();

		if (rightPage == -1)
			break;
		getExistedPage(rightPage, pageHandle);
		LDB(pageHandle.GetData(pageData));
		leafNode = &(((NodePagePacket*) pageData)->leafNode);
	}
	if (!find)
		ridPositionInfo.ridPagePos = -1;
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
				getExistedPage(ridPositionInfo.leafNode.data[++ridPositionInfo.posInLeaf].ridPageNum, pageHandle);
				ridPositionInfo.value = getValueFromRecRID(ridPositionInfo.leafNode.data[ridPositionInfo.posInLeaf].recRID);
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
					ridPositionInfo.value = getValueFromRecRID(ridPositionInfo.leafNode.data[0].recRID);
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
				ridPositionInfo.value = getValueFromRecRID(ridPositionInfo.leafNode.data[ridPositionInfo.posInLeaf].recRID);
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
					ridPositionInfo.value = getValueFromRecRID(ridPositionInfo.leafNode.data[0].recRID);
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

void IX_IndexHandle::addDisposedPage(PageNum pageNum) {
	disposedPageList[disposedPageNum++] = pageNum;
}

void IX_IndexHandle::unpinAllPages() {
	//printf("pinnedPageNum = %d\n", pinnedPageNum);
	for (int i = 0; i < pinnedPageNum; ++i) {
		LDB(fileHandle.UnpinPage(pinnedPageList[i]));
	}
	pinnedPageNum = 0;
}

void IX_IndexHandle::disposeAllPages() {

	bool rebuild = false;
	for (int i = 0; i < disposedPageNum; ++i) {
		if (disposedPageList[i] == indexInfo->rootPageNum)
			rebuild = true;
		LDB(fileHandle.DisposePage(disposedPageList[i]));
	}

	disposedPageNum = 0;

	if (rebuild) {
		PF_PageHandle pageHandle;
		PageNum pageNum;
		char *pageData;
		LDB(fileHandle.AllocatePage(pageHandle));
		LDB(pageHandle.GetPageNum(pageNum));
		LDB(pageHandle.GetData(pageData));
		NodePagePacket* nodePagePacket = (NodePagePacket*) pageData;
		nodePagePacket->nodeType = LEAF_NODE;
		(nodePagePacket->leafNode).init();
		LDB(fileHandle.MarkDirty(pageNum));
		LDB(fileHandle.ForcePages(pageNum));
		LDB(fileHandle.UnpinPage(pageNum));
		indexInfo->rootPageNum = pageNum;
		LDB(fileHandle.MarkDirty(0));
		LDB(fileHandle.ForcePages(0));
	}

}

RC IX_IndexHandle::ForcePages() {
	LDB(fileHandle.ForcePages());
	return 0;
}

int IX_IndexHandle::cmp(const void *a, const void *b) {
	if (a == nullptr || b == nullptr)
		return 0;
	if (indexInfo->attrType == AttrType::INT) {
		int _a = *(int*)a;
		int _b = *(int*)b;
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

int IX_IndexHandle::cmp(const RID ridA, const RID ridB) {
	char *aChar = getValueFromRecRID(ridA);
	char *bChar = getValueFromRecRID(ridB);
	void *a = (void*)aChar;
	void *b = (void*)bChar;
	int result = cmp(a, b);
	delete[] aChar;
	delete[] bChar;
	return result;
}
char* IX_IndexHandle::getValueFromRecRID(const RID rid) {
	char *tupleHead;
	RM_Record rmRecord;
	LDB(rmFileHandle->getRec(rid, rmRecord));
	tupleHead = rmRecord.getData();
	char *store = new char[indexInfo->attrLength + 1];
	memcpy(store, tupleHead + attrOffset, indexInfo->attrLength);
	return store;
}
