#include "ix_internal.h"
#include "ix.h"

RC RIDPagePacket::insertRID(const RID rid) {
	bool notCover[RID_BUCKET_SIZE] = {};
	for (int i = 0; i < poolSize; ++i)
		notCover[pool[i]] = true;
	for (int i = 0; i < nextOccp; ++i)
		if (!notCover[i] && (rid.getPageNum() == r[i].getPageNum()) && (rid.getSlotNum() == r[i].getSlotNum()))
			return IX_ENTRY_EXISTS;
	int size = nextOccp - poolSize;
	if (size < RID_BUCKET_SIZE) {
		if (poolSize > 0)
			r[pool[--poolSize]] = rid;
		else
			r[nextOccp++] = rid;
		return 0;
	}
	else
		return IX_BUCKET_FULL;
}

RC RIDPagePacket::deleteRID(const RID rid) {
	bool notCover[RID_BUCKET_SIZE] = {};
	for (int i = 0; i < poolSize; ++i)
		notCover[pool[i]] = true;
	for (int i = 0; i < nextOccp; ++i)
		if (!notCover[i] && (rid.getPageNum() == r[i].getPageNum()) && (rid.getSlotNum() == r[i].getSlotNum())) {
			pool[poolSize++] = i;
			return 0;
		}
	return IX_ENTRY_DOES_NOT_EXIST;
}

void LeafNode::split(LeafNode* splitNode, PageNum newPage, PageNum thisPage) {
	size = (NODE_KEYS + 2) >> 1;
	splitNode->size = (NODE_KEYS + 1) >> 1;
	for (int i = 0; i < splitNode->size; ++i)
		splitNode->data[i] = data[size + i];
	rightPage = newPage;
	splitNode->leftPage = thisPage;
	splitNode->rightPage = rightPage;
}

void LeafNode::insertDataIntoPos(void *pdata, PageNum pageNum, int pos) {
	for (int i = size; i >= pos + 1; --i)
		data[i] = data[i - 1];
	data[pos].pdata = pdata;
	data[pos].ridPageNum = pageNum;
	++size;
}

void InternalNode::InsertKeyAfterPos(void *pdata, PageNum pageNum, int pos) {
	for (int i = keyCount + 1; i >= pos + 2; --i) {
		son[i] = son[i - 1];
		pData[i] = pData[i - 1];
	}
	son[pos + 1] = pageNum;
	pData[pos + 1] = pdata;
	++keyCount;
}	

void InternalNode::Split(InternalNode* splitNode) {
	keyCount = (NODE_KEYS + 1) >> 1;
	splitNode->keyCount = (NODE_KEYS) >> 1;
	splitNode->son[0] = son[keyCount];
	for (int i = 1; i <= splitNode->keyCount; ++i) {
		splitNode->son[i] = son[keyCount + i];
		splitNode->pData[i] = pData[keyCount + i];
	}
}

