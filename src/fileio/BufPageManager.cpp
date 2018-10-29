//
// Created by Zhengxiao Du on 2018/10/29.
//

#include "../utils/MyHashMap.h"
#include "FindReplace.h"
#include "../utils/pagedef.h"
#include "../utils/MyLinkList.h"
#include "BufPageManager.h"

BufType BufPageManager::allocMem()
{
    return new unsigned int[(PAGE_SIZE >> 2)];
}

BufType BufPageManager::fetchPage(int typeID, int pageID, int &index)
{
    BufType b;
    index = replace->find();
    b = addr[index];
    if (b == NULL)
    {
        b = allocMem();
        addr[index] = b;
    } else
    {
        if (dirty[index])
        {
            int k1, k2;
            hash->getKeys(index, k1, k2);
            fileManager->writePage(k1, k2, b, 0);
            dirty[index] = false;
        }
    }
    hash->replace(index, typeID, pageID);
    return b;
}

BufPageManager::BufPageManager(FileManager *fm)
{
    int c = CAP;
    int m = MOD;
    last = -1;
    fileManager = fm;
    //bpl = new MyLinkList(CAP, MAX_FILE_NUM);
    dirty = new bool[CAP];
    addr = new BufType[CAP];
    hash = new MyHashMap(c, m);
    replace = new FindReplace(c);
    for (int i = 0; i < CAP; ++i)
    {
        dirty[i] = false;
        addr[i] = NULL;
    }
}

BufType BufPageManager::allocPage(int fileID, int pageID, int &index, bool ifRead)
{
    BufType b = fetchPage(fileID, pageID, index);
    if (ifRead)
    {
        fileManager->readPage(fileID, pageID, b, 0);
    }
    return b;
}

BufType BufPageManager::getPage(int fileID, int pageID, int &index)
{
    index = hash->findIndex(fileID, pageID);
    if (index != -1)
    {
        access(index);
        return addr[index];
    } else
    {
        BufType b = fetchPage(fileID, pageID, index);
        fileManager->readPage(fileID, pageID, b, 0);
        return b;
    }
}

void BufPageManager::access(int index)
{
    if (index == last)
    {
        return;
    }
    replace->access(index);
    last = index;
}

void BufPageManager::markDirty(int index)
{
    dirty[index] = true;
    access(index);
}

void BufPageManager::release(int index)
{
    dirty[index] = false;
    replace->free(index);
    hash->remove(index);
}

void BufPageManager::writeBack(int index)
{
    if (dirty[index])
    {
        int f, p;
        hash->getKeys(index, f, p);
        fileManager->writePage(f, p, addr[index], 0);
        dirty[index] = false;
    }
    replace->free(index);
    hash->remove(index);
}

void BufPageManager::close()
{
    for (int i = 0; i < CAP; ++i)
    {
        writeBack(i);
    }
}

void BufPageManager::getKey(int index, int &fileID, int &pageID)
{
    hash->getKeys(index, fileID, pageID);
}

BufPageManager &BufPageManager::getInstance()
{
    static BufPageManager instance = BufPageManager(&FileManager::getInstance());
    return instance;
}

BufPageManager::~BufPageManager()
{
    close();
}