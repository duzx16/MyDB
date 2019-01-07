#ifndef PRINT_ERR_H
#define PRINT_ERR_H

#include <cstdio>
#include <cstring>
void printErr(RC code, const char* msg) {
	if (code != 0)
		printf("code = %d msg = %s\n", code, msg);
}

#endif