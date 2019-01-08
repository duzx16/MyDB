#include "ix.h"
#include "ix_internal.h"
#include <cstdio>

IX_IndexScan::IX_IndexScan() = default;

IX_IndexScan::~IX_IndexScan() = default;

RC IX_IndexScan::OpenScan(IX_IndexHandle &_indexHandle, CompOp compOp, const void *value, ClientHint clientHint) {
	indexHandle = &_indexHandle;
	EQ_OP = false;
	skipValue = nullptr;
	if (compOp == CompOp::EQ_OP) {
		indexHandle->GetGeqRIDPos(value, ridPositionInfo, false);
		if (ridPositionInfo.ridPagePos != -1) {
			if (indexHandle->cmp(indexHandle->getValueFromRecRID(ridPositionInfo.leafNode.data[ridPositionInfo.posInLeaf].recRID), value) != 0)
				ridPositionInfo.ridPagePos = -1;
		}
		EQ_OP = true;
	}
	else if (compOp == CompOp::LT_OP) {
		indexHandle->GetGeqRIDPos(value, ridPositionInfo, false);
		//if (indexHandle->cmp(ridPositionInfo.leafNode.data[ridPositionInfo.posInLeaf].pdata, value) == 0)
		//	indexHandle->GetNextRIDPositionInfo(ridPositionInfo, -1, false);
		skipValue = value;
		dir = -1;
	}
	else if (compOp == CompOp::LE_OP) {
		indexHandle->GetGeqRIDPos(value, ridPositionInfo, false);
		dir = -1;
	}
	else if (compOp == CompOp::GT_OP) {
		indexHandle->GetGeqRIDPos(value, ridPositionInfo, false);
		//if (indexHandle->cmp(ridPositionInfo.leafNode.data[ridPositionInfo.posInLeaf].pdata, value) == 0)
		//	indexHandle->GetNextRIDPositionInfo(ridPositionInfo, 1, false);
		skipValue = value;
		dir = 1;
	}
	else if (compOp == CompOp::GE_OP) {
		indexHandle->GetGeqRIDPos(value, ridPositionInfo, false);
		dir = 1;
	}
	else if (compOp == CompOp::NE_OP) {
		indexHandle->GetGeqRIDPos(value, ridPositionInfo, true);
		skipValue = value;
		dir = 1;
	}
	else if (compOp == CompOp::NO_OP) {
		indexHandle->GetGeqRIDPos(value, ridPositionInfo, true);
		dir = 1;
	}
	ridHead = nullptr;
	RIDList *tail = nullptr;
	RID rid;
	while (ridPositionInfo.getCurRID(rid) != -1) {
		//printf("i = %d\n", i++);
		if (skipValue != nullptr && indexHandle->cmp(skipValue, ridPositionInfo.value) == 0)
			continue;
		auto *p = new RIDList();
		p->rid = rid;
		if (ridHead == nullptr) {
			ridHead = p;
			tail = p;
		}
		else {
			tail->next = p;
			tail = p;
		}
		indexHandle->GetNextRIDPositionInfo(ridPositionInfo, dir, EQ_OP);
	}
	return 0;
}

RC IX_IndexScan::GetNextEntry(RID &rid) {
	if (ridHead == nullptr) {
		return IX_EOF;
	}
	rid = ridHead->rid;
	RIDList *p = ridHead->next;
	delete ridHead;
	ridHead = p;
	return 0;
}

RC IX_IndexScan::CloseScan() {
	return 0;
}