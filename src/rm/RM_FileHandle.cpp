//
// Created by Zhengxiao Du on 2018/10/28.
//

#include "RM_FileHandle.h"
#include "../fileio/BufPageManager.h"
#include "RecordManager.h"
#include "../utils/MyBitMap.h"

int RM_FileHandle::getRec(const RID &rid, RM_Record &record) const
{
    BufPageManager &page_manager = BufPageManager::getInstance();
    int index;
    BufType page = page_manager.getPage(_fileID, rid.getPageNum(), index);
    record.init(reinterpret_cast<char *>(page) + getOffset(rid.getSlotNum()), _header_page.recordSize, rid);
    return 0;
}

RID RM_FileHandle::insertRec(const char *pData)
{
    if (_header_page.firstSparePage <= 0)
    {
        insertPage();
    }
    unsigned page_num = _header_page.firstSparePage;
    BufPageManager &page_manager = BufPageManager::getInstance();
    int index;
    BufType page = page_manager.getPage(_fileID, page_num, index);
    if (index == 1)
    {
        int a = 0;
    }

    page_manager.markDirty(index);
    printf("%d\n", index);
    MyBitMap bitmap(_header_page.slotMapSize * 8, page + 2);
    unsigned slot_num = bitmap.findLeftOne();
    memcpy(reinterpret_cast<char *>(page) + getOffset(slot_num), pData,
           _header_page.recordSize);
    bitmap.setBit(slot_num, 0);
    if (bitmap.findLeftOne() >= _header_page.recordPerPage)
    {
        if (static_cast<int>(page[1]) > 0)
        {
            _header_page.firstSparePage = page[1];
        } else
        {
            _header_page.firstSparePage = 0;
        }
        page[1] = 0;
    }
    _header_page.recordNum += 1;
    _header_modified = true;
    return {page_num, slot_num};
}

int RM_FileHandle::deleteRec(const RID &rid)
{
    BufPageManager &page_manager = BufPageManager::getInstance();
    int index;
    BufType page = page_manager.getPage(_fileID, rid.getPageNum(), index);
    MyBitMap bitmap(_header_page.slotMapSize * 8, page + 2);
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
            page[1] = (_header_page.firstSparePage);
        } else
        {
            page[1] = 0;
        }
        _header_page.firstSparePage = rid.getPageNum();
        _header_modified = true;
    }
    return 0;
}

int RM_FileHandle::updateRec(const RM_Record &rec)
{
    BufPageManager &page_manager = BufPageManager::getInstance();
    int index;
    BufType page = page_manager.getPage(_fileID, rec.getRID().getPageNum(), index);
    memcpy(reinterpret_cast<char *>(page) + getOffset(rec.getRID().getSlotNum()), rec.getData(),
           _header_page.recordSize);
    page_manager.markDirty(index);
    return 0;
}

int RM_FileHandle::insertPage()
{
    BufPageManager &page_manager = BufPageManager::getInstance();
    int index;
    BufType page = page_manager.getPage(_fileID, _header_page.pageNum, index);
    page[0] = _header_page.firstSparePage;
    _header_page.firstSparePage = _header_page.pageNum;
    memset(page + 2, 0xff, _header_page.slotMapSize);
    page_manager.markDirty(index);

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

