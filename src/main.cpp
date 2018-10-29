#include "rm/RecordManager.h"
#include "rm/RM_FileHandle.h"
#include "fileio/BufPageManager.h"
#include "utils/MyBitMap.h"
#include <unistd.h>


int main()
{
    MyBitMap::initConst();
    chdir("/Users/Duzx/Downloads/DB_test");
    RecordManager &rm = RecordManager::getInstance();
    RM_FileHandle file_handle;
    rm.createFile("persons", 10);
    rm.openFile("persons", file_handle);
    RM_FileHandle file_handle2;
    rm.createFile("class", 10);
    rm.openFile("class", file_handle2);
    for (int j = 0; j < 200; ++j)
    {
        RID rid = file_handle.insertRec("12345678901");
        RM_Record record = file_handle.getRec(rid);
    }


    for (int i = 0; i < 7; ++i)
    {
        RID new_rid(1, i);
        printf(file_handle.getRec(new_rid).getData());
        printf("\n");
    }
    rm.closeFile(file_handle);
    rm.closeFile(file_handle2);
}