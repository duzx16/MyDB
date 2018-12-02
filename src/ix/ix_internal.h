#ifndef IX_INTERNAL_H
#define IX_INTERNAL_H

#include "../Constants.h"
#include "../rm/RID.h"
#include "../pf/pf.h"

#define NODE_KEYS 6
#define RID_BUCKET_SIZE 300

struct IndexInfo {
	AttrType attrType;
	int attrLength;
	PageNum rootPageNum;
};
struct LeafData {
	void *pdata;
	PageNum ridPageNum;
	void init() { ridPageNum = -1; }
};
struct LeafNode {
	int size;
	LeafData data[NODE_KEYS + 1];
	PageNum leftPage, rightPage;
	void init() { size = 0; leftPage = rightPage = -1; }
	void insertDataIntoPos(void *pData, PageNum pageNum, int pos);
	void split(LeafNode* splitNode, PageNum newPage, PageNum thisPage);
};
struct InternalNode {
	int keyCount;
	PageNum son[NODE_KEYS + 2];
	void init() { keyCount = 0; for (int i = 0; i < NODE_KEYS + 2; ++i) son[i] = -1; }
	void* pData[NODE_KEYS + 2];
	void InsertKeyAfterPos(void *pData, PageNum pageNum, int pos);
	void ChangeKey(void *pData, int pos);
	void Split(InternalNode* splitNode);
};
struct NodePagePacket {
	int nodeType;
	/* 0 -> InternalNode 1 -> LeafNode */
	LeafNode leafNode;
	InternalNode internalNode;
};
struct RIDPagePacket {
	int nextOccp;
	int pool[RID_BUCKET_SIZE];
	int poolSize;
	RID r[RID_BUCKET_SIZE];
	RC insertRID(const RID rid);
	RC deleteRID(const RID rid);
};


#endif