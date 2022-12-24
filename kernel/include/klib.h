#pragma once

typedef unsigned int size_t;
typedef unsigned long long uint64_t;
typedef unsigned int uint32_t;
typedef unsigned short uint16_t;
typedef unsigned char uint8_t;

typedef int int32_t;

// extern so compiler can use them too
extern "C"
{
    size_t strlen(const char* string);
    char* strncpy(char* dest, const char* src, size_t maxLength);
    void strcpy(char* dest, const char* source);
    size_t strcmp(const char* x, const char* y);
    uint64_t pow(const uint64_t number, const uint64_t power);
    void memset(void* b, int c, size_t len);
    void memcpy(void *dest, void *src, size_t n); // has to be size_t as compiler uses it internally
    void memcpy_large(void *dest, void *src, uint64_t n);
    void __cxa_pure_virtual();
    void atexit();
}

// To keep intellisense happy
#ifndef SOURCE_PATH_SIZE
    #define SOURCE_PATH_SIZE 0
#endif

#define assert(expr) \
    if (!(expr)) \
        error((char const*)__FILE__ + SOURCE_PATH_SIZE, __LINE__, #expr)

#define println(message, ...) \
    _println((char const*)__FILE__ + SOURCE_PATH_SIZE, message, ##__VA_ARGS__)

void error(const char* file, unsigned int line, const char* expression);
void _println(const char* file, const char* message);
void _println(const char* file, const char* message, const char* message_two);
void _println(const char* file, const char* message, const int number);
void _println(const char* file, const char* message, const uint32_t number);
void _println(const char* file, const char* message, const uint64_t number);
void _println(const char* file, const char* message, const uint64_t number, const char* message_two, const uint64_t number_two);

#define MAX(a,b) \
   ({ const __typeof__ (a) _a = (a); \
       const __typeof__ (b) _b = (b); \
     _a > _b ? _a : _b; })

#define MIN(a,b) \
   ({ const __typeof__ (a) _a = (a); \
       const __typeof__ (b) _b = (b); \
     _a < _b ? _a : _b; })


template<typename T>
struct Optional
{
    T data;
    bool contains_data;

    Optional(T _data) : data(_data), contains_data(true) {}
    Optional() : data(), contains_data(false) {}

    T operator*() const
    {
        assert(contains_data);
        return data;
    }

    const T* operator->() const
    {
        assert(contains_data);
        return &data;
    }

    bool operator!() const
    {
        return !contains_data;
    }

    T value_or(T or_value) const
    {
        if (contains_data) return data;
        return or_value;
    }
};

#define halt() asm volatile("hlt")
