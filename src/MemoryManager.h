#pragma once

#ifdef DEBUG
#include "Log.h"

#include "Map.h"

class MemoryManager {
public:
    static MemoryManager* getInstance();

	const bool hasLeak() const;
    void displayInfo();
    void* mallocMemory(size_t size, bool isArr);
    void freeMemory(void* p, bool isArr);

private:
    size_t _padding, _paddingSize, _memoryUsage, _maxMemoryUsage, _maxPointers;
    Map<void*, size_t> _map, _mapArr;

    static MemoryManager* _pInstance;

    void init();

    void add(Map<void*, size_t> &map, void* p, size_t size);

    void remove(Map<void*, size_t> &map, void* p);

    //Add padding at end of pointer. Used to detect heap corruption.
    void addPadding(void *p, size_t size);

    //Check if padding is still intact. Used to detect heap corruption.
    void checkPadding(void *p, size_t size);

};

/* ------------------------------------------------ */

void* operator new(size_t size);
void* operator new [](size_t size);
void operator delete(void * p);
void operator delete [](void * p);

#endif