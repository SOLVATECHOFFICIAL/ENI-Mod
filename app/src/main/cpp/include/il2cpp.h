#ifndef IL2CPP_H
#define IL2CPP_H

#include <stdint.h>
#include <stddef.h>

// IL2CPP type definitions
typedef int32_t il2cpp_array_size_t;

struct Il2CppObject {
    void* vtable;
    void* monitor;
};

struct Il2CppString {
    Il2CppObject object;
    int32_t length;
    uint16_t chars[1];
};

struct Il2CppArray {
    Il2CppObject object;
    il2cpp_array_size_t max_length;
    void* vector[1];
};

// Vector types
struct Vector2 {
    float x, y;
    Vector2() : x(0), y(0) {}
    Vector2(float _x, float _y) : x(_x), y(_y) {}
};

struct Vector3 {
    float x, y, z;
    Vector3() : x(0), y(0), z(0) {}
    Vector3(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {}
    
    Vector3 operator-(const Vector3& other) const {
        return Vector3(x - other.x, y - other.y, z - other.z);
    }
    Vector3 operator+(const Vector3& other) const {
        return Vector3(x + other.x, y + other.y, z + other.z);
    }
    Vector3 operator*(float scalar) const {
        return Vector3(x * scalar, y * scalar, z * scalar);
    }
    
    float Distance(const Vector3& other) const {
        float dx = x - other.x;
        float dy = y - other.y;
        float dz = z - other.z;
        return __builtin_sqrt(dx*dx + dy*dy + dz*dz);
    }
    
    float AngleTo(const Vector3& target) const;
};

struct Vector4 {
    float x, y, z, w;
    Vector4() : x(0), y(0), z(0), w(0) {}
    Vector4(float _x, float _y, float _z, float _w) : x(_x), y(_y), z(_z), w(_w) {}
};

struct Matrix4x4 {
    float m[16];
    Vector4 operator*(const Vector4& v) const;
};

struct Color {
    float r, g, b, a;
    Color() : r(1), g(1), b(1), a(1) {}
    Color(float _r, float _g, float _b, float _a) : r(_r), g(_g), b(_b), a(_a) {}
};

// Bone IDs
enum BoneID {
    BONE_ROOT = 0,
    BONE_SPINE = 7,
    BONE_HEAD = 10,
    BONE_NECK = 9,
    BONE_LEFT_HAND = 25,
    BONE_RIGHT_HAND = 30,
    BONE_LEFT_FOOT = 5,
    BONE_RIGHT_FOOT = 2
};

// Player class forward declaration
class Player;

// IL2CPP function pointers
typedef void* (*il2cpp_domain_get_t)();
typedef void* (*il2cpp_domain_assembly_open_t)(void* domain, const char* name);
typedef void* (*il2cpp_class_from_name_t)(void* image, const char* namespaze, const char* name);
typedef void* (*il2cpp_class_get_method_from_name_t)(void* klass, const char* name, int argsCount);
typedef void* (*il2cpp_object_new_t)(void* klass);
typedef void* (*il2cpp_resolve_icall_t)(const char* name);

// Function pointers
extern il2cpp_domain_get_t il2cpp_domain_get;
extern il2cpp_domain_assembly_open_t il2cpp_domain_assembly_open;
extern il2cpp_class_from_name_t il2cpp_class_from_name;
extern il2cpp_class_get_method_from_name_t il2cpp_class_get_method_from_name;
extern il2cpp_object_new_t il2cpp_object_new;
extern il2cpp_resolve_icall_t il2cpp_resolve_icall;

// Game function pointers
typedef void* (*GetLocalPlayer_t)();
typedef void* (*GetPlayerList_t)();
typedef Vector3 (*GetBonePosition_t)(void* player, int boneId);
typedef bool (*IsVisible_t)(Vector3 from, Vector3 to);

// Global game functions
extern GetLocalPlayer_t GetLocalPlayer;
extern GetPlayerList_t GetPlayerList;
extern GetBonePosition_t GetBonePosition;
extern IsVisible_t IsVisible;

// Screen dimensions (set from ESP)
extern int ScreenWidth;
extern int ScreenHeight;

// View/Projection matrices
Matrix4x4 GetViewMatrix();
Matrix4x4 GetProjectionMatrix();

// Init
bool InitIL2CPP(void* il2cppHandle);
void* GetIL2CPPMethod(const char* className, const char* methodName, int argsCount);

// Hooking
bool A64HookFunction(void* symbol, void* replace, void** result);
void* GetSymbolAddress(const char* libName, const char* symbolName);

#endif // IL2CPP_H
