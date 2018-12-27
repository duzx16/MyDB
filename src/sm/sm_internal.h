#idndef SM_INTERNAL_H
#define SM_INTERNAL_H

#include "sm.h"

#define MAX_ATTRS 21
struct TableCons {
	ConstraintType type;
    ConstValueList const_values;
    IdentList column_list;
    std::string column_name;
    std::string foreign_table;
    std::string foreign_column;
};
struct TableInfo {
	int attrInfoCount;
	AttrInfo attrInfos[MAX_ATTRS];
	int indexedAttrSize;
	int indexedAttr[MAX_ATTRS];
	int tableConsCount;
	TableCons tableCons[MAX_ATTRS];
};
#endif