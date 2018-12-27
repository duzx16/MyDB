#ifndef SM_H
#define SM_H

#include "../Constants.h"
#include "../parser/Tree.h"
#include "../pf/pf.h"
#include "../ix/ix.h"
#include "../rm/RecordManager.h"


class SM_Manager {
public:
    SM_Manager  ();  // Constructor
    ~SM_Manager ();                                  // Destructor
	RC CreateDb    (const char *dbName);                // Create database
	RC DropDb      (const char *dbName);                // Drop database
    RC OpenDb      (const char *dbName);                // Open database
    RC CloseDb     ();                                  // Close database
	RC CreateTable (const char *tableName,
					ColumnDecsList *columns,
					TableConstraintList *tableConstraints);
	RC GetTableInfo(const char *tableName,
					ColumnDecsList &columns,
					TableConstraintList &tableConstraints);				
    RC DropTable   (const char *relName);               // Destroy relation
    RC CreateIndex (const char *relName,                // Create index
                    const char *attrName);
    RC DropIndex   (const char *relName,                // Destroy index
                    const char *attrName);
    RC Load        (const char *relName,                // Load utility
                    const char *fileName);
    RC Help        ();                                  // Help for database
    RC Help        (const char *relName);               // Help for relation
    RC Print       (const char *relName);               // Print relation
	static SM_Manager* getInstance();
private:
	IX_Manager *ixm;
	RecordManager *rmm;
	PF_Manager pfManager;
	void GenerateTableMetadataDir(const char *tableName, char *s);
	void GenerateTableRecordDir(const char *tableName, char *s);
};

//
// Print-error function
//
void SM_PrintError(RC rc);


#define SM_REL_EXISTS            (START_SM_WARN + 0)
#define SM_REL_NOTEXIST          (START_SM_WARN + 1)
#define SM_ATTR_NOTEXIST         (START_SM_WARN + 2)
#define SM_INDEX_EXISTS          (START_SM_WARN + 3)
#define SM_INDEX_NOTEXIST        (START_SM_WARN + 4)
#define SM_FILE_FORMAT_INCORRECT (START_SM_WARN + 5)
#define SM_FILE_NOT_FOUND        (START_SM_WARN + 6)
#define SM_LASTWARN SM_FILE_NOT_FOUND


#define SM_CHDIR_FAILED    (START_SM_ERR - 0)
#define SM_CATALOG_CORRUPT (START_SM_ERR - 1)
#define SM_LASTERROR SM_CATALOG_CORRUPT

#endif