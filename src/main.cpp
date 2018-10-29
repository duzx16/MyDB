#include "rm/RecordManager.h"
#include "rm/RM_FileHandle.h"
#include "fileio/BufPageManager.h"
#include <unistd.h>



int main()
{
    chdir("/Users/Duzx/Downloads/DB_test");
    RecordManager &rm = RecordManager::getInstance();
    RM_FileHandle file_handle;
    rm.createFile("persons", 10);
    rm.openFile("persons", file_handle);
    RM_FileHandle file_handle2;
    rm.createFile("class", 10);
    rm.openFile("class", file_handle2);

    RID rid = file_handle.insertRec("12345678901");
    RM_Record record = file_handle.getRec(rid);
    BufPageManager::getInstance().close();
    printf(record.getData());
}