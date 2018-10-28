//
// Created by Zhengxiao Du on 2018/10/28.
//

#include "RecordManager.h"


RecordManager::RecordManager(FileManager &file_manager) : _file_manager(file_manager),
                                                          _page_manager(&file_manager)
{
}

RecordManager::~RecordManager()
{

}

int RecordManager::createFile(std::string file_name, unsigned record_size)
{
    int rc;
    if ((rc = _file_manager.createFile(file_name.data())))
    {
        return rc;
    }
    // Create the Heade Page
    int file_id, index;
    if((rc = _file_manager.openFile(file_name.data(), file_id))){
        return rc;
    }
    auto header = reinterpret_cast<HeaderPage *>(_page_manager.allocPage(file_id, 0, index, false));

    //Set up the header page
    header->pageNum = 0;
    header->recordSize = record_size;
    header->pageNum = 1;
    header->recordNum = PAGE_SIZE / record_size;
    header->firstSparePage = 0;

    //Write back the header page
    _page_manager.markDirty(index);
    _page_manager.writeBack(index);

    //Close the file
    _file_manager.closeFile(file_id);
    return 0;
}

int RecordManager::destroyFile(std::string filename)
{
    return 0;
}

int RecordManager::openFile(std::string filename, RM_FileHandle &file_handle)
{
    return 0;
}

int RecordManager::closeFile(RM_FileHandle &file_handle)
{
    return 0;
}
