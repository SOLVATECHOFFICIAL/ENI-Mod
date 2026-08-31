#ifndef UTILS_H
#define UTILS_H

#include <stdint.h>
#include <stddef.h>

// Memory utilities
bool MemoryPatch(void* address, const void* patch, size_t size);
bool MemoryWrite(void* address, uint32_t value);
bool MemoryRead(void* address, void* buffer, size_t size);

// Pattern scanning
void* FindPatternInLib(const char* libName, const char* pattern, const char* mask);
void* FindPattern(void* start, size_t length, const char* pattern, const char* mask);

// Hook utilities
#define HOOK(target, hook, original) \
    A64HookFunction((void*)(target), (void*)(hook), (void**)&(original))

// Math
float CalcAngleDelta(Vector3 from, Vector3 to);
Vector3 CalcAngle(Vector3 from, Vector3 to);

// Safe memory read
template<typename T>
T ReadMemory(void* address) {
    T value;
    memcpy(&value, address, sizeof(T));
    return value;
}

template<typename T>
void WriteMemory(void* address, T value) {
    memcpy(address, &value, sizeof(T));
}

#endif // UTILS_H
