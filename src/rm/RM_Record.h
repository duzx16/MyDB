//
// Created by Zhengxiao Du on 2018/10/28.
//

#ifndef MYDB_RM_RECORD_H
#define MYDB_RM_RECORD_H

#include <cstring>
#include "RID.h"
#include <stdio.h>

struct RM_Record {
public:
    RM_Record() : _data(nullptr), _size(0) {

    }

    RM_Record(const char *data, unsigned size, const RID &rid) : _rid(rid), _size(size) {
        _data = new char[size];
        memcpy(_data, data, size);
    }

    RM_Record(const RM_Record &record) : RM_Record(record._data, record._size, record._rid) {}

    RM_Record(RM_Record &&record) noexcept {
        _data = record._data;
        _size = record._size;
        _rid = record._rid;
        record._data = nullptr;
        record._size = 0;
    }


    RID getRID() const {
        return _rid;
    }

    char *getData() const {
        return _data;
    }

    unsigned getSize() const {
        return _size;
    }

    void init(const char *data, unsigned size, const RID &rid) {
        this->_rid = rid;

        delete[]_data;

        _data = new char[size];
        memcpy(_data, data, size);
    }

    ~RM_Record() {
        delete[]_data;

    }

private:
    RID _rid;
    char *_data;
    unsigned _size;
};

#endif //MYDB_RM_RECORD_H
