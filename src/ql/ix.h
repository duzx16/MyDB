//
// Created by Zhengxiao Du on 2018-12-23.
//

//
// ix.h
//
//   Index Manager Component Interface
//

#ifndef IX_H
#define IX_H

// Please do not include any other files than the ones below in this file.

#include "../Constants.h"
#include "../rm/RID.h"
#include "../pf/pf.h"

class IX_IndexHandle;

//
// IX_Manager: provides IX index file management
//
class IX_Manager {
    PF_Manager *pfm;

    // RM_Manager rmm;

public:
    IX_Manager   (PF_Manager &pfm);              // Constructor
    ~IX_Manager  ();                             // Destructor
    RC CreateIndex  (const char *fileName,          // Create new index
                     int        indexNo,
                     AttrType   attrType,
                     int        attrLength);
    RC DestroyIndex (const char *fileName,          // Destroy index
                     int        indexNo);
    RC OpenIndex    (const char *fileName,          // Open index
                     int        indexNo,
                     IX_IndexHandle &indexHandle);
    RC CloseIndex   (IX_IndexHandle &indexHandle);  // Close index
    static IX_Manager & getInstance();
private:
    char* GenFileName(const char* filename, int indexNo);
};

//
// IX_IndexHandle: IX Index File interface
//
class IX_IndexHandle {
public:
    IX_IndexHandle  ();                             // Constructor
    ~IX_IndexHandle ();                             // Destructor
    RC InsertEntry     (void *pData, const RID &rid);  // Insert new index entry
    RC DeleteEntry     (void *pData, const RID &rid);  // Delete index entry
    RC ForcePages      ();                             // Copy index to disk

    void init(const char* indexFileName, PF_Manager *_pfm);
    void CloseIndex();

    void PrintFullLinkList(); // print full link list just for attrtype = int
};

//
// IX_IndexScan: condition-based scan of index entries
//
class IX_IndexScan {
public:
    IX_IndexScan  ();                                 // Constructor
    ~IX_IndexScan ();                                 // Destructor
    RC OpenScan      (const IX_IndexHandle &indexHandle, // Initialize index scan
                      CompOp      compOp,
                      void        *value);
    RC GetNextEntry  (RID &rid);                      // Get next matching entry
    RC CloseScan     ();     	// Terminate index scan
};

//
// Print-error function
//
void IX_PrintError(RC rc);

#define IX_EOF                  (START_IX_WARN + 0)
#define IX_ENTRY_EXISTS         (START_IX_WARN + 1)
#define IX_ENTRY_DOES_NOT_EXIST (START_IX_WARN + 2)
#define IX_SCAN_NOT_OPENED      (START_IX_WARN + 3)
#define IX_SCAN_NOT_CLOSED      (START_IX_WARN + 4)
#define IX_BUCKET_FULL          (START_IX_WARN + 5)
#define IX_LASTWARN IX_SCAN_NOT_CLOSED


#define IX_ATTR_TOO_LARGE       (START_IX_ERR - 0)
#define IX_LASTERROR IX_ATTR_TOO_LARGE

#endif // IX_H
