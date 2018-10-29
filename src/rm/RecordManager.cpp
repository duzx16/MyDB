//
// Created by Zhengxiao Du on 2018/10/28.
//

#include "RecordManager.h"
#include "../fileio/FileManager.h"
#include "../fileio/BufPageManager.h"


unsigned bitmap_size(unsigned record_per_page)
{
    return ((record_per_page + 31u) >> 5u) * 4;
}

unsigned record_per_page(unsigned record_size)
{
    return (32 * PAGE_SIZE - 31) / (record_size * 32 + 1);
}

RecordManager::RecordManager()
{
}

RecordManager::~RecordManager()
{

}

int RecordManager::createFile(std::string filename, unsigned record_size)
{
    int rc;
    FileManager &file_manager = FileManager::getInstance();
    BufPageManager &page_manager = BufPageManager::getInstance();
    if (rc = file_manager.createFile(filename.c_str()))
    {
        return rc;
    }
    // Create the Heade Page
    int file_id, index;
    if (rc = file_manager.openFile(filename.c_str(), file_id))
    {
        return rc;
    }
    auto header = reinterpret_cast<HeaderPage *>(page_manager.allocPage(file_id, 0, index, false));

    //Set up the header page
    header->recordSize = record_size;
    header->pageNum = 1;
    header->recordNum = 0;
    header->recordPerPage = record_per_page(record_size);
    header->firstSparePage = 0;
    header->slotMapSize = bitmap_size(header->recordPerPage) + 4;

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
    int rc;
    FileManager &file_manager = FileManager::getInstance();
    BufPageManager &page_manager = BufPageManager::getInstance();

    // Create the Heade Page
    int file_id, index;
    if (rc = file_manager.openFile(filename.c_str(), file_id))
    {
        return rc;
    }
    auto header = reinterpret_cast<HeaderPage *>(page_manager.allocPage(file_id, 0, index, true));
    file_handle._header_page = *header;
    file_handle._header_modified = false;
    file_handle._fileID = file_id;
    file_handle._initialized = true;

    // Release the Header Page
    page_manager.release(index);
    return 0;
}

int RecordManager::closeFile(RM_FileHandle &file_handle)
{
    int rc;
    if (file_handle._header_modified)
    {
        int index;
        auto header = reinterpret_cast<HeaderPage *>(BufPageManager::getInstance().allocPage(file_handle._fileID, 0,
                                                                                             index, false));
        *header = file_handle._header_page;
        BufPageManager::getInstance().markDirty(index);
        BufPageManager::getInstance().writeBack(index);
    }

    //Write back the pages
    BufPageManager::getInstance().closeFile(file_handle._fileID);

    //Close the file
    if ((rc = FileManager::getInstance().closeFile(file_handle._fileID)) != 0)
    {
        return rc;
    }

    file_handle._initialized = false;
    return 0;
}

RecordManager &RecordManager::getInstance()
{
    static RecordManager instance;
    return instance;
}
