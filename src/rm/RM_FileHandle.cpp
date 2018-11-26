//
// Created by Zhengxiao Du on 2018/10/28.
//

#include "RM_FileHandle.h"
#include "RecordManager.h"
#include "../utils/MyBitMap.h"

int RM_FileHandle::getRec(const RID &rid, RM_Record &record) const
{
    int rc;
    PF_PageHandle page_handle;
    if((rc = _pf_file_handle.GetThisPage(rid.getPageNum(), page_handle)) != 0)
    {
        PF_PrintError(rc);
        return -1;
    }
    char *page_data;
    page_handle.GetData(page_data);
    record.init(reinterpret_cast<char *>(page_data) + getOffset(rid.getSlotNum()), _header_page.recordSize, rid);
    _pf_file_handle.UnpinPage(rid.getPageNum());
    return 0;
}

RID RM_FileHandle::insertRec(const char *pData)
{
    int rc;
    if (_header_page.firstSparePage <= 0)
    {
        insertPage();
    }
    unsigned page_num = _header_page.firstSparePage;
    PF_PageHandle page_handle;
    if((rc = _pf_file_handle.GetThisPage(page_num, page_handle)) != 0){
        PF_PrintError(rc);
    }
    char *page_data;
    page_handle.GetData(page_data);
    _pf_file_handle.MarkDirty(page_num);

    MyBitMap bitmap(_header_page.slotMapSize * 8, reinterpret_cast<unsigned *>(page_data + 8));
    unsigned slot_num = bitmap.findLeftOne();
    memcpy(page_data + getOffset(slot_num), pData,
           _header_page.recordSize);
    bitmap.setBit(slot_num, 0);
    if (bitmap.findLeftOne() >= _header_page.recordPerPage)
    {
        if (reinterpret_cast<int *>(page_data)[1] > 0)
        {
            _header_page.firstSparePage = reinterpret_cast<int *>(page_data)[1];
        } else
        {
            _header_page.firstSparePage = 0;
        }
        reinterpret_cast<int *>(page_data)[1] = 0;
    }

    if ((rc = _pf_file_handle.UnpinPage(page_num)) != 0)
    {
        printf("%d\n", rc);
    }
    _header_page.recordNum += 1;
    _header_modified = true;
    return {page_num, slot_num};
}

int RM_FileHandle::deleteRec(const RID &rid)
{
    int rc;
    PF_PageHandle page_handle;
    if((rc = _pf_file_handle.GetThisPage(rid.getPageNum(), page_handle)) != 0)
    {
        PF_PrintError(rc);
        return -1;
    }
    char *page_data;
    page_handle.GetData(page_data);
    MyBitMap bitmap(_header_page.slotMapSize * 8, reinterpret_cast<unsigned *>(page_data + 8));
    bool full = false;
    if (bitmap.findLeftOne() >= _header_page.recordPerPage)
    {
        full = true;
    }
    bitmap.setBit(rid.getSlotNum(), 1);
    if (full)
    {
        if (_header_page.firstSparePage > 0)
        {
            reinterpret_cast<int *>(page_data)[1] = (_header_page.firstSparePage);
        } else
        {
            reinterpret_cast<int *>(page_data)[1] = 0;
        }
        _header_page.firstSparePage = rid.getPageNum();
        _header_modified = true;
    }
    _pf_file_handle.MarkDirty(rid.getPageNum());
    if ((rc = _pf_file_handle.UnpinPage(rid.getPageNum())) != 0)
    {
        PF_PrintError(rc);
        return -1;
    }

    return 0;
}

int RM_FileHandle::updateRec(const RM_Record &rec)
{

    int rc;
    PF_PageHandle page_handle;
    rc = _pf_file_handle.GetThisPage(rec.getRID().getPageNum(), page_handle);
    if(rc != 0)
    {
        PF_PrintError(rc);
    }
    char *page_data = nullptr;
    rc = page_handle.GetData(page_data);
    memcpy(page_data + getOffset(rec.getRID().getSlotNum()), rec.getData(),
           _header_page.recordSize);
    _pf_file_handle.MarkDirty(rec.getRID().getPageNum());
    if ((rc = _pf_file_handle.UnpinPage(rec.getRID().getPageNum())) != 0)
    {
        PF_PrintError(rc);
    }
    return 0;
}

int RM_FileHandle::insertPage()
{
    int rc;
    PF_PageHandle page_handle;
    if((rc = _pf_file_handle.AllocatePage(page_handle)) != 0)
    {
        PF_PrintError(rc);
        return -1;
    }
    char *page_data;
    page_handle.GetData(page_data);
    reinterpret_cast<int *>(page_data)[0] = _header_page.firstSparePage;
    _header_page.firstSparePage = _header_page.pageNum;
    memset(page_data + 8, 0xff, _header_page.slotMapSize);
    long page_num;
    page_handle.GetPageNum(page_num);
    _pf_file_handle.MarkDirty(page_num);
    if ((rc = _pf_file_handle.UnpinPage(page_num)) != 0)
    {
        return rc;
    }
    _header_page.pageNum += 1;
    _header_modified = true;
    return 0;
}

unsigned RM_FileHandle::getOffset(unsigned slot_num) const
{
    return 8 + _header_page.slotMapSize + slot_num * _header_page.recordSize;
}

RM_FileHandle::~RM_FileHandle()
{
    if (_initialized)
    {
        RecordManager::getInstance().closeFile(*this);
    }
}

