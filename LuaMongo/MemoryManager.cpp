#include "MemoryManager.h"

#ifdef DEBUG

MemoryManager* MemoryManager::_pInstance = nullptr;

/*************************************
*************** PUBLIC ***************
**************************************/

MemoryManager* MemoryManager::getInstance() {
    if (!_pInstance) {
        _pInstance = (MemoryManager*) malloc(sizeof (MemoryManager));
        _pInstance->init();
    }
    return _pInstance;
}

const bool MemoryManager::hasLeak() const {
	return _map.size() + _mapArr.size() > 0;
}

void MemoryManager::displayInfo() {
	if (_map.size() + _mapArr.size() > 0) {
		LOG_ERROR("*** MEMORY LEAK DETECTED! p: %lu, p[]: %lu ***", _map.size(), _mapArr.size());
	}
	LOG_INFO("Max memory usage: %.1fkB", _maxMemoryUsage / 1000.0);
	LOG_INFO("Max num pointers used: %lu", _maxPointers);
}

void* MemoryManager::mallocMemory(const size_t size, const bool isArr) {
	//Add 4 * padding to the size.
	void *p = malloc(size + _paddingSize * 4);
	//    size_t address = reinterpret_cast<size_t> (p);
	//    if (address == 0x643860) {
	//        int a = 2;
	//    }
	if (isArr) {
		add(_mapArr, p, size);
	}
	else {
		add(_map, p, size);
	}
	size_t numPointers = _mapArr.size() + _map.size();
	_maxPointers = _maxPointers > numPointers ? _maxPointers : numPointers;
	return p;
}

void MemoryManager::freeMemory(void*const p, const bool isArr) {
	if (isArr) {
		remove(_mapArr, p);
	}
	else {
		remove(_map, p);
	}
	free(p);
}

/*************************************
*************** PRIVATE **************
**************************************/

void MemoryManager::addPadding(void *const p, const size_t size) {
    //Copy padding data to the memory at the end.
    memcpy((char*) p + size, &_padding, _paddingSize);
    memcpy((char*) p + size + _paddingSize, &_padding, _paddingSize);
    memcpy((char*) p + size + _paddingSize * 2, &_padding, _paddingSize);
    memcpy((char*) p + size + _paddingSize * 3, &_padding, _paddingSize);
}

void MemoryManager::checkPadding(void *const p, const size_t size) {
    size_t *padding = (size_t*) ((char*) p + size);
    if (padding[0] != _padding || padding[1] != _padding || padding[2] != _padding || padding[3] != _padding) {
        LOG_ERROR("*** HEAP CORRUPTION DETECTED! ***");
    }
}

void MemoryManager::init() {
    _padding = 0xAAAAAAAA;
    _paddingSize = sizeof (_padding);
    _memoryUsage = _maxMemoryUsage = _maxPointers = 0;
    _map.init(500);
    _mapArr.init(500);
}

void MemoryManager::add(Map<void*, size_t> &map, void* p, size_t size) {
    if (map.contains(p)) {
        LOG_ERROR("***** POINTER ALREADY IN USE: %p *****", p);
    }

    _memoryUsage += size;
    if (_memoryUsage > _maxMemoryUsage) {
        _maxMemoryUsage = _memoryUsage;
    }

    //Add padding at end data.
    addPadding(p, size);

    //Store pointer in map.
    map.put(p, size);
}

void MemoryManager::remove(Map<void*, size_t> &map, void*const p) {
    //Get size from map.
    size_t size;
    if (!map.get(p, size)) {
        LOG_ERROR("*** CAN'T FIND POINTER: %p ***", p);
        return;
    }

    _memoryUsage -= size;

    //Check if padding is valid.
    checkPadding(p, size);

    //Remove pointer from map.
    map.remove(p);
}

/* ------------------------------------------------ */

void* operator new(const size_t size){
    if (!size) {
        LOG_ERROR("NEW SIZE IS ZERO");
    }
    return MemoryManager::getInstance()->mallocMemory(size, false);
}

void* operator new [](const size_t size) {
    if (!size) {
        LOG_ERROR("NEW[] SIZE IS ZERO");
    }
    return MemoryManager::getInstance()->mallocMemory(size, true);
}

void operator delete(void*const p) {
    MemoryManager::getInstance()->freeMemory(p, false);
}

void operator delete [](void*const p) {
    MemoryManager::getInstance()->freeMemory(p, true);
}

#endif