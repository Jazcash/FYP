#ifndef UTILITY_H_
#define UTILITY_H_

#include <stdlib.h>

void* operator new(size_t objsize) {
	return malloc(objsize);
}

void operator delete(void* obj) {
	free(obj);
}

int randomBetween(int a, int b){
	return rand() % ( b - a + 1) + a;
}

#endif /* UTILITY_H_ */