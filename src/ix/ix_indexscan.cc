#include "ix.h"

IX_IndexScan::IX_IndexScan() {}

IX_IndexScan::~IX_IndexScan() {}

RC IX_IndexScan::OpenScan(const IX_IndexHandle &_indexHandle, CompOp compOp, void *value, ClientHint clientHint) {
	indexHandle = _indexHandle;
	if (compOp == CompOp::EQ_OP) {
		indexHandle.GetGeqRIDPos(value, ridPositioninfo, false);
		if (cmp(ridPositionInfo.leafNode.data[ridPositionInfo.posInLeaf], value) != 0)
			ridPositionInfo.ridPagePos = -1;
		EQ_OP = true;
	}
	else if (compOp == CompOp::LT_OP) {
		indexHandle.GetGeqRIDPos(value, ridPositioninfo, false);
		if (cmp(ridPositionInfo.leafNode.data[ridPositionInfo.posInLeaf], value) == 0)
			indexHandle.GetNextRIDPositionInfo(ridPositionInfo, -1);
		dir = -1;
	}
	else if (compOp == CompOp::LE_OP) {
		indexHandle.GetGeqRIDPos(value, ridPositioninfo, false);
		dir = -1;
	}
	else if (compOp == CompOp::GT_OP) {
		indexHandle.GetGeqRIDPos(value, ridPositioninfo, false);
		if (cmp(ridPositionInfo.leafNode.data[ridPositionInfo.posInLeaf], value) == 0)
			indexHandle.GetNextRIDPositionInfo(ridPositionInfo, 1);
		dir = 1;
	}
	else if (compOp == CompOp::GE_OP) {
		indexHandle.GetGeqRIDPos(value, ridPositioninfo, false);
		dir = 1;
	}
	else if (compOp == CompOp::NE_OP) {
		indexHandle.GetGeqRIDPos(value, ridPositioninfo, true);
		skipValue = value;
		dir = 1;
	}
	else if (compOp == CompOp::NO_OP) {
		indexHandle.GetGeqRIDPos(value, ridPositioninfo, true);
		dir = 1;
	}
}

RC IX_IndexScan::GetNextEntry(RID &rid) {
	if (ridPositionInfo.ridPagePos == -1) {
		return IX_EOF;
	}
	// todo : gen full list before and output here
	rid = ridPositionInfo.getCurRID();
	indexHandle.GetNextRIDPositionInfo(ridPositionInfo, dir, EQ_OP);
}

RC IX_IndexScan::CloseScan() {}