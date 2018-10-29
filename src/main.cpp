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
    rm.openFile("persons", file_handle);
    RM_FileHandle file_handle2;
    rm.openFile("class", file_handle2);



    RID rid = file_handle.insertRec("12345678901");
    RM_Record record = file_handle.getRec(rid);
    for (int i = 0; i < 7; ++i)
    {
        RID new_rid(1,i);
        printf(file_handle.getRec(rid).getData());
        printf("\n");
    }
    BufPageManager::getInstance().close();
    printf(record.getData());
}