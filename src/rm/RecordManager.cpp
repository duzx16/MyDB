//
// Created by Zhengxiao Du on 2018/10/28.
//

#include "RecordManager.h"


unsigned bitmap_size(unsigned record_per_page) {
    return ((record_per_page + 31u) >> 5u) * 4;
}

unsigned record_per_page(unsigned record_size) {
    return (32 * (PF_PAGE_SIZE - 8) - 124) / (record_size * 32 + 4);
}

RecordManager::RecordManager() : _pf_manager(PF_Manager::getInstance()) {

}

int RecordManager::createFile(std::string filename, unsigned record_size) {
    int rc;
    rc = _pf_manager.CreateFile(filename.c_str());
    if (rc) {
        PF_PrintError(rc);
        return rc;
    }
    // Create the Heade Page
    PF_FileHandle file_handle;
    rc = _pf_manager.OpenFile(filename.c_str(), file_handle);
    if (rc) {
        PF_PrintError(rc);
        return rc;
    }
    PF_PageHandle page_handle;
    rc = file_handle.AllocatePage(page_handle);
    if (rc) {
        PF_PrintError(rc);
        return rc;
    }
    char *data;
    page_handle.GetData(data);
    auto header = reinterpret_cast<HeaderPage *>(data);

    //Set up the header page
    header->recordSize = record_size;
    header->pageNum = 1;
    header->recordNum = 0;
    header->recordPerPage = record_per_page(record_size);
    header->firstSparePage = 0;
    header->slotMapSize = bitmap_size(header->recordPerPage) + 4;

    //Write back the header page
    long page_num;
    page_handle.GetPageNum(page_num);
    rc = file_handle.MarkDirty(page_num);
    rc = file_handle.UnpinPage(page_num);
    if (rc) {
        PF_PrintError(rc);
        return rc;
    }

    //Close the file
    if ((rc = _pf_manager.CloseFile(file_handle)) != 0) {
        PF_PrintError(rc);
        return rc;
    }
    return 0;
}

int RecordManager::destroyFile(std::string filename) {
    return _pf_manager.DestroyFile(filename.c_str());
}

int RecordManager::openFile(std::string filename, RM_FileHandle &file_handle) {
    int rc;

    // Create the Heade Page
    if ((rc = _pf_manager.OpenFile(filename.c_str(), file_handle._pf_file_handle)) != 0) {
        return rc;
    }

    PF_PageHandle page_handle;
    file_handle._pf_file_handle.GetFirstPage(page_handle);
    char *data;
    page_handle.GetData(data);
    auto header = reinterpret_cast<HeaderPage *>(data);
    file_handle._header_page = *header;
    file_handle._header_modified = false;
    file_handle._initialized = true;

    // Release the Header Page
    if ((rc = file_handle._pf_file_handle.UnpinPage(0)) != 0) {
        PF_PrintError(rc);
        return rc;
    }

    return 0;
}

int RecordManager::closeFile(RM_FileHandle &file_handle) {
    int rc;
    if (file_handle._header_modified) {
        PF_PageHandle page_handle;
        file_handle._pf_file_handle.GetFirstPage(page_handle);
        char *data;
        page_handle.GetData(data);
        auto header = reinterpret_cast<HeaderPage *>(data);
        *header = file_handle._header_page;
        file_handle._pf_file_handle.MarkDirty(0);
        file_handle._pf_file_handle.UnpinPage(0);
    }

    //Close the file
    if ((rc = _pf_manager.CloseFile(file_handle._pf_file_handle)) != 0) {
        PF_PrintError(rc);
        return rc;
    }

    file_handle._initialized = false;
    return 0;
}

RecordManager &RecordManager::getInstance() {
    static RecordManager instance;
    return instance;
}
