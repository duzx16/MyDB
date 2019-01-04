#include "ix_internal.h"
#include "ix.h"
#include <cstdio>
#include <cstdlib>

RC RIDPagePacket::insertRID(const RID rid) {
	for (int i = 0; i < size; ++i) {
		if (rid.getPageNum() == r[i].getPageNum() && rid.getSlotNum() == r[i].getSlotNum()) {
			return IX_ENTRY_EXISTS;
		}
	}
	if (size + 1 == RID_BUCKET_SIZE) {
		return IX_BUCKET_FULL;
	}
	else {
		r[size++] = rid;
		return 0;
	}
}

RC RIDPagePacket::deleteRID(const RID rid) {
	int pos = -1;
	for (int i = 0; i < size; ++i) {
		if (rid.getPageNum() == r[i].getPageNum() && rid.getSlotNum() == r[i].getSlotNum()) {
			pos = i;
			break;
		}
	}
	if (pos == -1)
		return IX_ENTRY_DOES_NOT_EXIST;
	for (int i = pos; i < size - 1; ++i)
		r[i] = r[i + 1];
	--size;
	return 0;
}

void LeafNode::split(LeafNode* splitNode, PageNum newPage, PageNum thisPage) {
	size = D + 1;
	splitNode->size = D;
	for (int i = 0; i < splitNode->size; ++i)
		splitNode->data[i] = data[size + i];
	splitNode->leftPage = thisPage;
	splitNode->rightPage = rightPage;
	rightPage = newPage;
}

void LeafNode::insertDataIntoPos(void *pdata, PageNum pageNum, int pos) {
	for (int i = size; i >= pos + 1; --i)
		data[i] = data[i - 1];
	data[pos].pdata = pdata;
	data[pos].ridPageNum = pageNum;
	++size;
}

void LeafNode::deleteDataAtPos(int pos) {
	for (int i = pos; i <= size - 2; ++i)
		data[i] = data[i + 1];
	--size;
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
	keyCount = D;
	splitNode->keyCount = D - 1;
	splitNode->son[0] = son[D + 1];
	for (int i = 1; i <= splitNode->keyCount; ++i) {
		splitNode->son[i] = son[D + 1 + i];
		splitNode->pData[i] = pData[D + 1 + i];
	}
}

void LineDebug(int line, const char *file, int err) {
	if (err != 0) {
		printf("line %d in file %s, err = %d\n", line, file, err);
		if (err < 0) {
			exit(0);
		}
	}
}

void LeafNode::operator = (const LeafNode &o) {
	size = o.size;
	for (int i = 0; i < size; ++i)
		data[i] = o.data[i];
	leftPage = o.leftPage;
	rightPage = o.rightPage;
}

void RIDPagePacket::operator = (const RIDPagePacket &o) {
	size = o.size;
	for (int i = 0; i < size; ++i)
		r[i] = o.r[i];
}

RIDList::RIDList() {
	next = NULL;
}

int RIDPositionInfo::getCurRID(RID &rid) {
	if (ridPagePos == -1)
		return -1;
	rid = ridPagePacket.r[ridPagePos];
	return 0;
}