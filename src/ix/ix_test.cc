#include "ix.h"
#include "ix_internal.h"
#include "../rm/RecordManager.h"
#include <cstdlib>
#include <ctime>
#include <cassert>
#include <algorithm>

char recFile[110] = "TestRecordFile";
char indFile[110] = "TestIndexFile";
int v[100010];
RID r[100010];
std::pair<int, int> stp[100010];
const int n = 10000;
const int m = n / 2;
const int half_m = m / 2;
const int gap = n / 10;
int validID[100010];
void checkResultList(IX_IndexHandle &indexHandle, CompOp compOp, void *value) {
	RID rid;
	for (int i = 0; i < n; ++i)
		stp[i] = std::make_pair(v[i], i);
	std::sort(stp, stp + n);
	int count = 0;
	for (int i = 0; i < n; ++i) {
		bool cond = false;
		switch (compOp) {
			case CompOp::LE_OP: {
				cond = indexHandle.cmp((void*)(&(stp[i].first)), value) <= 0;
				break;
			}
			case CompOp::LT_OP: {
				cond = indexHandle.cmp((void*)(&(stp[i].first)), value) < 0;
				break;
			}
			case CompOp::GE_OP: {
				cond = indexHandle.cmp((void*)(&(stp[i].first)), value) >= 0;
				break;
			}
			case CompOp::GT_OP: {
				cond = indexHandle.cmp((void*)(&(stp[i].first)), value) > 0;
				break;
			}
			case CompOp::NO_OP: {
				cond = true;
				break;
			}
			case CompOp::EQ_OP: {
				cond = indexHandle.cmp((void*)(&(stp[i].first)), value) == 0;
				break;
			}
			case CompOp::NE_OP: {
				cond = indexHandle.cmp((void*)(&(stp[i].first)), value) != 0;
				break;
			}
			default: {
				break;
			}
		}
		if (cond)
			validID[count++] = stp[i].second;
	}
	int pointer = 0;
	IX_IndexScan indexScan;
	LDB(indexScan.OpenScan(indexHandle, compOp, value, ClientHint::NO_HINT));
	while (indexScan.GetNextEntry(rid) != IX_EOF) {
		//printf("expected = %d get = %d\n", v[validID[pointer]], *(int*)indexHandle.getValueFromRecRID(rid));
		assert(v[validID[pointer++]] == *(int*)indexHandle.getValueFromRecRID(rid));
	}
	assert(count == pointer);
}

void IX_Test() {
	int seed = time(NULL);
	//int seed = 1547018327;
	printf("seed = %d\n", seed);
	//srand(time(NULL));
	srand(seed);
	PF_Manager &pfManager = PF_Manager::getInstance();
	IX_Manager *ixm = &IX_Manager::getInstance();
	RecordManager *rmm = &RecordManager::getInstance();
	LDB(rmm->destroyFile(recFile));
	LDB(rmm->createFile(recFile, sizeof (int) + 1));
	RM_FileHandle rmFileHandle;
	// insert into rec file
	LDB(rmm->openFile(recFile, rmFileHandle));
	RID rid;
	for (int i = 0; i < n; ++i) {
		v[i] = rand() % m + 1;
		//v[i] = 100000 + i;
		rid = rmFileHandle.insertRec((char*)(v + i));
		r[i] = rid;
		//printf("i = %d, vi = %d, ri = (%ld, %u)\n", i, v[i], r[i].getPageNum(), r[i].getSlotNum());
	}
	LDB(rmm->closeFile(rmFileHandle));
	// insert into index
	IX_IndexHandle indexHandle;
	LDB(ixm->DestroyIndex(indFile, 0));
	LDB(ixm->CreateIndex(indFile, 0, AttrType::INT, 4));
	LDB(rmm->openFile(recFile, rmFileHandle));
	LDB(ixm->OpenIndex(indFile, 0, indexHandle, rmFileHandle, 0));
	RM_FileScan rmFileScan;
	LDB(rmFileScan.openScan(rmFileHandle, AttrType::INT, 4, 0, CompOp::ISNOT_OP, nullptr));
	RM_Record rmRecord;
	int count = 0;
	while (rmFileScan.getNextRec(rmRecord) == 0) {
		if (count % gap == 0)
			printf("insert index #%d\n", count);
		rid = rmRecord.getRID();
		char *value = rmRecord.getData();
		assert(*(int*)value == v[count]);
		assert(rid.getPageNum() == r[count].getPageNum() && rid.getSlotNum() == r[count].getSlotNum());
		count++;
		indexHandle.InsertEntry(rid);
		//indexHandle.checkLinkListIsValid();
	}
	checkResultList(indexHandle, CompOp::LE_OP, new int(half_m));
	checkResultList(indexHandle, CompOp::LT_OP, new int(half_m));
	checkResultList(indexHandle, CompOp::GE_OP, new int(half_m));
	checkResultList(indexHandle, CompOp::GT_OP, new int(half_m));
	printf("test after insertion passed...\n");
	LDB(rmFileScan.closeScan());
	LDB(ixm->CloseIndex(indexHandle));
	LDB(rmm->closeFile(rmFileHandle));
	
	LDB(rmm->openFile(recFile, rmFileHandle));
	LDB(ixm->OpenIndex(indFile, 0, indexHandle, rmFileHandle, 0));
	for (int i = 0; i < n; ++i) {
		if (i % gap == 0)
			printf("update entry #%d\n", i);
		/*
		if (i == 17) {
			printf("i see you~\n");
		}
		*/
		// gen update pos and value
		//int pos = rand() % n;
		//int value = rand();
		int pos = i;
		int value = -v[i];
		//printf("set pos %d = %d\n", pos, value);
		// remove old index
		LDB(indexHandle.DeleteEntry(r[pos]));
		// get record and modify
		RM_Record rec;
		LDB(rmFileHandle.getRec(r[pos], rec));
		char *recData = rec.getData();
		int *intRecData = (int*) recData;
		*intRecData = value;
		v[pos] = value;
		// update record
		LDB(rmFileHandle.updateRec(rec));
		// check value is updated
		LDB(rmFileHandle.getRec(r[pos], rec));
		recData = rec.getData();
		assert(*(int*)recData == v[pos]);
		
		/*
		// delete record
		LDB(rmFileHandle.deleteRec(rid));
		// insert record
		v[pos] = value;
		LDB(rmFileHandle.insertRec((char*)(v + pos), rid);
		r[pos] = rid;
		*/
		
		// insert new index
		LDB(indexHandle.InsertEntry(r[pos]));
		//checkResultList(indexHandle);
	}
	// check result is correct
	checkResultList(indexHandle, CompOp::LE_OP, new int(-m * 2));
	checkResultList(indexHandle, CompOp::LE_OP, new int(0));
	checkResultList(indexHandle, CompOp::GE_OP, new int(-m * 2));
	checkResultList(indexHandle, CompOp::GE_OP, new int(0));
	checkResultList(indexHandle, CompOp::EQ_OP, new int(-half_m));
	checkResultList(indexHandle, CompOp::NE_OP, new int(-half_m));
	checkResultList(indexHandle, CompOp::NO_OP, NULL);
	printf("test after update passed...\n");
	LDB(ixm->CloseIndex(indexHandle));
	LDB(rmm->closeFile(rmFileHandle));
}