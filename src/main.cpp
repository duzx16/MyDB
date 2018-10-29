#include "rm/RecordManager.h"
#include "rm/RM_FileHandle.h"
#include "rm/RM_FileScan.h"
#include "fileio/BufPageManager.h"
#include "utils/MyBitMap.h"
#include <unistd.h>
#include <vector>


int main()
{
    MyBitMap::initConst();
    chdir("/Users/Duzx/Downloads/DB_test");
    RecordManager &rm = RecordManager::getInstance();
    RM_FileHandle file_handle;
    //rm.createFile("persons", 10);
    rm.openFile("persons", file_handle);
//    RM_FileHandle file_handle2;
//    rm.createFile("class", 10);
//    rm.openFile("class", file_handle2);
    RM_Record record;
    std::vector<RID> rids;
    for (int j = 0; j < 2000; ++j)
    {
        RID rid = file_handle.insertRec(std::to_string(j).c_str());
        file_handle.getRec(rid, record);
//        if(j % 2 == 0 and j < 100) {
//            rids.push_back(rid);
//        }
    }
//    for(auto &it: rids){
//        file_handle.deleteRec(it);
//    }
    RM_FileScan fileScan;
    fileScan.openScan(file_handle, AttrType::NO_ATTR, 0, 0, CompOp::NO_OP, nullptr);
    int i = 0;
    while(true)
    {
        int rc = fileScan.getNextRec(record);
        if(rc) {
            break;
        }
//        printf(record.getData());
//        printf("\n");
        i += 1;
        if(i > 199) {
            int a = 0;
        }
    }
    rm.closeFile(file_handle);
    //rm.closeFile(file_handle2);
}