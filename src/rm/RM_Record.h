//
// Created by Zhengxiao Du on 2018/10/28.
//

#ifndef MYDB_RM_RECORD_H
#define MYDB_RM_RECORD_H

#include <cstring>
#include "RID.h"

struct RM_Record {
public:
    RM_Record() : _data(nullptr), _size(0)
    {

    }

    RM_Record(const char *data, unsigned size, const RID &rid) : _rid(rid)
    {
        _data = new char[size];
        memcpy(_data, data, size);
    }

    RID getRID() const
    {
        return _rid;
    }

    char *getData() const
    {
        return _data;
    }

    unsigned getSize() const
    {
        return _size;
    }

    void init(const char *data, unsigned size, const RID &rid)
    {
        this->_rid = rid;
        if (_data)
        {
            delete[]_data;
        }
        _data = new char[size];
        memcpy(_data, data, size);
    }

    ~RM_Record()
    {
        if (_data)
        {
            delete[]_data;
        }
    }

private:
    RID _rid;
    char *_data;
    unsigned _size;
};

#endif //MYDB_RM_RECORD_H
