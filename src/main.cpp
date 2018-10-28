#include "rm/RecordManager.h"
#include <unistd.h>

int main()
{
    chdir("/Users/Duzx/Downloads");
    RecordManager &rm = RecordManager::getInstance();
    rm.createFile("persons", 10);
}