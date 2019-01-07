#include "ix/ix.h"
#include "pf/pf.h"
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <unistd.h>
#include <fcntl.h>
#include <cassert>
#include <vector>

#define LDB(opt) LineDebug(__LINE__, __FILE__, opt)

void pfManagerTest(PF_Manager *pfManager) {
	LDB(pfManager->DestroyFile("PF_Manager_Test_File"));
	LDB(pfManager->CreateFile("PF_Manager_Test_File"));
	PF_FileHandle fileHandle;
	LDB(pfManager->OpenFile("PF_Manager_Test_File", fileHandle));
	PF_PageHandle pageHandle;
	LDB(fileHandle.AllocatePage(pageHandle));
	LDB(fileHandle.UnpinPage(0));
	LDB(fileHandle.GetThisPage(0, pageHandle));
	LDB(fileHandle.GetThisPage(0, pageHandle));
	LDB(fileHandle.GetThisPage(0, pageHandle));
	LDB(fileHandle.UnpinPage(0));
	LDB(fileHandle.UnpinPage(0));
	LDB(fileHandle.UnpinPage(0));
	LDB(pfManager->CloseFile(fileHandle));
}

void readTest() {
	int fd = open("newFile.76543", O_BINARY | O_RDWR);
	char a[4096 * 20];
	int num = read(fd, a, 8);
	assert(num == 8);
	close(fd);
}
int valueStore[100010];
std::vector<int> s[40000];
int main() {
	readTest();
	//return 0;
	//srand(time(NULL));
	PF_Manager *pfManager = new PF_Manager();
	pfManagerTest(pfManager);
	//return 0;
    IX_Manager *ixManager = new IX_Manager(pfManager);
	ixManager->DestroyIndex("newFile", 34567);
	ixManager->CreateIndex("newFile", 34567, AttrType::INT, 4);
	IX_IndexHandle indexHandle;
	ixManager->OpenIndex("newFile", 34567, indexHandle);
	//ixManager->CloseIndex(indexHandle);
	for (int i = 0; i < 100000; ++i) {
		int r = rand();
		valueStore[i] = r;
		indexHandle.InsertEntry((void*)(valueStore + i), RID(i, i));
		s[r].push_back(i);
		//printf("i = %d, v = %d\n", i, r);
	}
	ixManager->CloseIndex(indexHandle);
	
	ixManager->OpenIndex("newFile", 34567, indexHandle);
	for (int i = 0; i <= 32767; i++) {
		//printf("del val = %d\n", i);
		if (i == 16171) {
			//printf("here...\n");
		}
		for (auto id : s[i]) {
			int *p = new int(i);
			indexHandle.DeleteEntry((void*)p, RID(id, id));
			delete p;
		}
	}
	//printf("clear ok\n");
	ixManager->CloseIndex(indexHandle);
	
	ixManager->OpenIndex("newFile", 34567, indexHandle);
	for (int i = 0; i < 100000; ++i) {
		int r = rand();
		valueStore[i] = r;
		//printf("insert value = %d, i = %d\n", r, i);
		indexHandle.InsertEntry((void*)(valueStore + i), RID(i, i));
		s[r].push_back(i);
	}
	
	indexHandle.PrintFullLinkList();
	
	IX_IndexScan indexScan;
	indexScan.OpenScan(indexHandle, CompOp::NO_OP, NULL, ClientHint::NO_HINT);
	RID rid;
	while (indexScan.GetNextEntry(rid) != IX_EOF) {
		//printf("pageNum = %u slotNum = %u value = %d\n", rid.getPageNum(), rid.getSlotNum(), valueStore[rid.getPageNum()]);
	}
	indexScan.CloseScan();
	
	ixManager->CloseIndex(indexHandle);
	printf("I'm alive now...\n");
	return 0;
}