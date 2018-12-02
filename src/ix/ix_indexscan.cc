#include "ix.h"

IX_IndexScan::IX_IndexScan() {}

IX_IndexScan::~IX_IndexScan() {}

RC IX_IndexScan::OpenScan(const IX_IndexHandle &indexHandle, CompOp compOp, void *value, ClientHint clientHint) {
	PageNum leafPage = indexHandle.FindLeafPage(value);
	if (compOp == CompOp::EQ_OP || compOp == CompOp::NO_OP) {
		
	}
	else if (compOp == CompOp::LT_OP || compOp == CompOp::LE_OP) {
		
	}
	else if (compOp == CompOp::GT_OP || compOp == CompOp::GE_OP) {
		
	}
	else {// CompOp::NE_OP
		
	}
}

RC IX_IndexScan::GetNextEntry(RID &rid) {}

RC IX_IndexScan::CloseScan() {}