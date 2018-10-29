//
// Created by Zhengxiao Du on 2018/10/28.
//

#ifndef MYDB_RID_H
#define MYDB_RID_H

class RID {
public:
    RID() = default;
    RID(unsigned page_num, unsigned slot_num) : _page_num(page_num), _slot_num(slot_num) {}

    // Construct RID from page and
    //   slot number
    unsigned getPageNum() const { return _page_num; }  // Return page number
    unsigned getSlotNum() const { return _slot_num; }  // Return slot number
private:
    unsigned _page_num = 0;
    unsigned _slot_num = 0;
};

#endif //MYDB_RID_H
