#ifndef REDISNATIVE_MEMORYHELPER_H
#define REDISNATIVE_MEMORYHELPER_H

#include <types.h>
#include <IMemoryManager.h>

template<typename T>
bool copyString(IMemoryManager* iMemory, T** destination, const T* source, size_t size) {

    constexpr auto type_size = sizeof(T);
    if (!iMemory->AllocMemory((void**)destination, size * type_size)) {
        return false;
    }
    memcpy_s(*destination, size * type_size, source, size * type_size);
    return true;
}
#endif //REDISNATIVE_MEMORYHELPER_H
