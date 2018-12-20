//
// Created by Zhengxiao Du on 2018/10/29.
//

#include "RM_FileScan.h"
#include "RM_FileHandle.h"
#include "../pf/pf.h"

int
RM_FileScan::openScan(const RM_FileHandle &fileHandle, Expr *condition) {
    _file_handle = &fileHandle;
    _condition = condition;
    _current_page = 0;
    _current_bitdata = nullptr;
    return 0;
}


int RM_FileScan::getNextRec(RM_Record &rec) {
    while (true) {
        unsigned slot_num = _file_handle->_header_page.recordPerPage + 1;
        if (_current_page != 0) {
            slot_num = _current_bitmap.findLeftOne();
        }
        while (slot_num >= _file_handle->_header_page.recordPerPage) {
            _current_page += 1;
            if (_current_page >= _file_handle->_header_page.pageNum) {
                return 1;
            }
            PF_PageHandle page_handle;
            _file_handle->_pf_file_handle.GetThisPage(_current_page, page_handle);
            char *page_data;
            page_handle.GetData(page_data);
            delete[]_current_bitdata;
            _current_bitdata = new char[_file_handle->_header_page.slotMapSize];
            memcpy(_current_bitdata, page_data + 8, _file_handle->_header_page.slotMapSize);
            _file_handle->_pf_file_handle.UnpinPage(_current_page);
            for (int i = 0; i < _file_handle->_header_page.slotMapSize; ++i) {
                _current_bitdata[i] = ~_current_bitdata[i];
            }
            _current_bitmap = MyBitMap(_file_handle->_header_page.slotMapSize * 8,
                                       reinterpret_cast<unsigned *>(_current_bitdata));
            slot_num = _current_bitmap.findLeftOne();
        }
        _file_handle->getRec(RID{_current_page, slot_num}, rec);
        _current_bitmap.setBit(slot_num, 0);
        char *data = rec.getData();
        _condition->calculate(data);
        if (_condition->value.b) {
            break;
        }
    }
    return 0;
}

int RM_FileScan::closeScan() {
    delete[] _current_bitdata;
    return 0;
}
