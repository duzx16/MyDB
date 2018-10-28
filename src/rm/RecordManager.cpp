//
// Created by Zhengxiao Du on 2018/10/28.
//

#include "RecordManager.h"
#include "../fileio/FileManager.h"
#include "../fileio/BufPageManager.h"


RecordManager::RecordManager()
{
}

RecordManager::~RecordManager()
{

}

int RecordManager::createFile(std::string file_name, unsigned record_size)
{
    int rc;
    FileManager & file_manager = FileManager::getInstance();
    BufPageManager & page_manager = BufPageManager::getInstance();
    if (rc = file_manager.createFile(file_name.c_str()))
    {
        return rc;
    }
    // Create the Heade Page
    int file_id, index;
    if (rc = file_manager.openFile(file_name.c_str(), file_id))
    {
        return rc;
    }
    auto header = reinterpret_cast<HeaderPage *>(page_manager.allocPage(file_id, 0, index, false));

    //Set up the header page
    header->pageNum = 0;
    header->recordSize = record_size;
    header->pageNum = 1;
    header->recordNum = PAGE_SIZE / record_size;
    header->firstSparePage = 0;

    //Write back the header page
    page_manager.markDirty(index);
    page_manager.writeBack(index);

    //Close the file
    if (rc = file_manager.closeFile(file_id))
    {
        return rc;
    }
    return 0;
}

int RecordManager::destroyFile(std::string filename)
{
    return FileManager::getInstance().deleteFile(filename.c_str());
}

int RecordManager::openFile(std::string filename, RM_FileHandle &file_handle)
{

    return 0;
}

int RecordManager::closeFile(RM_FileHandle &file_handle)
{
    return 0;
}

RecordManager &RecordManager::getInstance()
{
    static RecordManager instance;
    return instance;
}
