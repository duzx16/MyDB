#include "ix/ix.h"
#include "pf/pf.h"
#include <cstdio>

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
int main() {
	PF_Manager *pfManager = new PF_Manager();
	pfManagerTest(pfManager);
	//return 0;
    IX_Manager *ixManager = new IX_Manager(*pfManager);
	ixManager->DestroyIndex("newFile", 34567);
	ixManager->CreateIndex("newFile", 34567, AttrType::INT, 4);
	IX_IndexHandle *indexHandle = new IX_IndexHandle();
	ixManager->OpenIndex("newFile", 34567, *indexHandle);
	for (int i = 0; i < 30; ++i) {
		printf("i = %d\n", i);
		indexHandle->InsertEntry(new int(i), RID(i, i));
		//indexHandle->PrintFullLinkList();
	}
	
	IX_IndexScan indexScan;
	indexScan.OpenScan(*indexHandle, CompOp::NO_OP, NULL, ClientHint::NO_HINT);
	RID rid;
	while (indexScan.GetNextEntry(rid) != IX_EOF) {
		printf("pageNum = %u slotNum = %u\n", rid.getPageNum(), rid.getSlotNum());
	}
	indexScan.CloseScan();
	
	ixManager->CloseIndex(*indexHandle);
	printf("I'm alive now...\n");
	return 0;
}