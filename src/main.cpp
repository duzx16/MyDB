#include "rm/RecordManager.h"
#include "rm/RM_FileHandle.h"
#include <unistd.h>

int main()
{
    chdir("/Users/Duzx/Downloads/DB_test");
    RecordManager &rm = RecordManager::getInstance();
    RM_FileHandle file_handle;
    rm.openFile("persons", file_handle);
    file_handle.setRecordSize(12);
    rm.closeFile(file_handle);
}