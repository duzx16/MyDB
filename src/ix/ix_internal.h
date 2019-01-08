#ifndef IX_INTERNAL_H
#define IX_INTERNAL_H

#include "../Constants.h"
#include "../rm/RID.h"
#include "../pf/pf.h"


const int D = (4050 - 16) / (sizeof(RID) + sizeof(PageNum)) / 4 - 3;
const int RID_BUCKET_SIZE = 4000 / sizeof(RID);
#define MAX_DEPTH 1010
#define INTERNAL_NODE 0
#define LEAF_NODE 1
#define LDB(opt) LineDebug(__LINE__, __FILE__, opt)
struct IndexInfo {
	AttrType attrType;
	int attrLength;
	PageNum rootPageNum;
};
struct LeafData {
	//void *pdata;
	RID recRID;
	PageNum ridPageNum;
	void init() { ridPageNum = -1; }
};
struct LeafNode {
	int size;
	LeafData data[D * 2 + 1];
	PageNum leftPage, rightPage;
	void init() { size = 0; leftPage = rightPage = -1; }
	//void insertDataIntoPos(void *pData, PageNum pageNum, int pos);
	void insertDataIntoPos(RID rid, PageNum pageNum, int pos);
	void deleteDataAtPos(int pos);
	void split(LeafNode* splitNode, PageNum newPage, PageNum thisPage);
	void operator = (const LeafNode &);
};
struct InternalNode {
	int keyCount;
	PageNum son[2 * D + 2];
	RID recRID[2 * D + 2];
	void init() { keyCount = 0; for (int i = 0; i <= 2 * D; ++i) son[i] = -1; }
	//void InsertKeyAfterPos(void *pData, PageNum pageNum, int pos);
	void InsertKeyAfterPos(RID rid, PageNum pageNum, int pos);
	void DeleteKeyAtPos(int pos);
	void Split(InternalNode* splitNode);
};
struct NodePagePacket {
	int nodeType;
	/* 0 -> InternalNode 1 -> LeafNode */
	LeafNode leafNode;
	InternalNode internalNode;
};
struct RIDPagePacket {
	int size;
	RID r[RID_BUCKET_SIZE];
	RC insertRID(const RID rid);
	RC deleteRID(const RID rid);
	void operator = (const RIDPagePacket &);
};
struct RIDPositionInfo {
	LeafNode leafNode;
	int posInLeaf;
	RIDPagePacket ridPagePacket;
	int ridPagePos;
	void *value;
	int getCurRID(RID &rid);
};
struct RIDList {
	RID rid;
	RIDList *next;
	RIDList();
};

void LineDebug(int line, const char *file, int err);
#endif
