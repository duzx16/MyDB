//
// Created by Zhengxiao Du on 2018/10/28.
//

#ifndef MYDB_HEADERPAGE_H
#define MYDB_HEADERPAGE_H

struct HeaderPage {
    unsigned recordSize;
    unsigned recordNum;
    unsigned recordPerPage;
    unsigned firstSparePage;
    unsigned pageNum;
};

#endif //MYDB_HEADERPAGE_H
