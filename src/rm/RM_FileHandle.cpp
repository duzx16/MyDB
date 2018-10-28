//
// Created by Zhengxiao Du on 2018/10/28.
//

#include "RM_FileHandle.h"

int RM_FileHandle::getRec(const RID &rid, RM_Record &rec) const
{
    return 0;
}

int RM_FileHandle::insertRec(const char *pData, RID &rid)
{
    return 0;
}

int RM_FileHandle::deleteRec(const RID &rid)
{
    return 0;
}

int RM_FileHandle::updateRec(const RM_Record &rec)
{
    return 0;
}

int RM_FileHandle::forcePages(unsigned pageNum) const
{
    return 0;
}

