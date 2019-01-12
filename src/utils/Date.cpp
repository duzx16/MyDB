//
// Created by Zhengxiao Du on 2019-01-01.
//

#include "Date.h"
#include <stdio.h>

bool IsLeapYear(int year) {
    return ((year % 4 == 0) && (year % 100 != 0)) || (year % 400 == 0);
}

bool IsLegalDate(int year, int mon, int day) {
    //大：1 3 5 7 8 10 12
    //小：4 6 9 11
    //平：2

    if (year < 0 || mon <= 0 || mon > 12 || day <= 0 || day > 31)return false;

    if (1 == mon || 3 == mon || 5 == mon || 7 == mon || 8 == mon || 10 == mon || 12 == mon) {
        return true;
    }
    if (mon != 2) {
        return day != 31;
    }
    if (IsLeapYear(year)) {
        return !(30 == day || 31 == day);
    } else {
        return !(29 == day || 30 == day || 31 == day);
    }
}

int parseData(const char *date) {
    int year, month, day;
    char *str;
    year = static_cast<int>(strtol(date, &str, 10));
    if (*str == '-') {
        str++;
        month = static_cast<int>(strtol(str, &str, 10));
        if (*str == '-') {
            str++;
            day = static_cast<int>(strtol(str, &str, 10));
            if (IsLegalDate(year, month, day)) {
                return (year - 1) * 31 * 12 + (month - 1) * 31 + (day - 1);
            }
        }
    }
    return -1;

}
