#include "ix/ix.h"
#include "pf/pf.h"
#include <cstdio>

int main() {
	PF_Manager *pfManager = new PF_Manager();
    IX_Manager *ixManager = new IX_Manager(*pfManager);
	ixManager->CreateIndex("newFile", 34567, AttrType::INT, 4);
	IX_IndexHandle *indexHandle = new IX_IndexHandle();
	ixManager->OpenIndex("newFile", 34567, *indexHandle);
	for (int i = 0; i < 10; ++i) {
		indexHandle->InsertEntry(new int(0), RID(i, i));
	}
	return 0;
}