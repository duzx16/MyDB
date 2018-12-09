#include "rm/RecordManager.h"
#include "rm/RM_FileHandle.h"
#include "rm/RM_FileScan.h"
#include "fileio/BufPageManager.h"
#include "utils/MyBitMap.h"
#include <unistd.h>
#include <vector>

char start_parse(const char *expr_input);
int yyparse();


//int main()
//{
//    MyBitMap::initConst();
//    chdir("/Users/Duzx/Downloads/DB_test");
//    RecordManager &rm = RecordManager::getInstance();
//    RM_FileHandle file_handle;
//    rm.createFile("persons", 10);
//    rm.openFile("persons", file_handle);
//    RM_Record record;
//    std::vector<RID> rid1, rid2;
//    for (int j = 0; j < 2000; ++j)
//    {
//        RID rid = file_handle.insertRec(std::to_string(j).c_str());
//        file_handle.getRec(rid, record);
//        if(j % 2 != 0) {
//            rid1.push_back(rid);
//        }
//        else if (j % 6 == 0){
//            rid2.push_back(rid);
//        }
//    }
//    for(auto &it: rid1){
//        file_handle.deleteRec(it);
//    }
//    for(auto &it: rid2){
//        file_handle.updateRec(RM_Record{"test", 10, it});
//    }
//    RM_FileScan fileScan;
//    fileScan.openScan(file_handle, AttrType::NO_ATTR, 0, 0, CompOp::NO_OP, nullptr);
//    int i = 0;
//    while(true)
//    {
//        int rc = fileScan.getNextRec(record);
//        if(rc) {
//            break;
//        }
//        printf(record.getData());
//        printf("\n");
//        i += 1;
//    }
//    rm.closeFile(file_handle);
//}

int main() {
    return yyparse();
}