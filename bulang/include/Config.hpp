#pragma once
  
typedef uint8_t  u8;
typedef int8_t   c8;
typedef uint16_t u16;
typedef int16_t  s16;
typedef uint32_t u32;
typedef int32_t  s32;
typedef int64_t  s64;
typedef uint64_t u64;

#if defined(_DEBUG)
#include <assert.h>
//#define DEBUG_BREAK_IF(_CONDITION_) assert(!(_CONDITION_));
#define DEBUG_BREAK_IF(condition) if (condition) { fprintf(stderr, "Debug break: %s at %s:%d\n", #condition, __FILE__, __LINE__); std::exit(EXIT_FAILURE); }
#else
#define DEBUG_BREAK_IF(_CONDITION_)
#endif

inline size_t CalculateCapacityGrow(size_t capacity, size_t minCapacity)
{
    if (capacity < minCapacity)
        capacity = minCapacity;
    if (capacity < 8)
    {
        capacity = 8;
    }
    else
    {
        // Round up to the next power of 2 and multiply by 2 (http://graphics.stanford.edu/~seander/bithacks.html#RoundUpPowerOf2)
        capacity--;
        capacity |= capacity >> 1;
        capacity |= capacity >> 2;
        capacity |= capacity >> 4;
        capacity |= capacity >> 8;
        capacity |= capacity >> 16;
        capacity++;
    }
    return capacity;
}





template <typename T>
class Allocator
{
public:
    T *allocate(size_t n)
    {
        return static_cast<T *>(::operator new(n * sizeof(T)));
    }

    void deallocate(T *p, size_t n)
    {
        ::operator delete(p, n * sizeof(T));
    }

    template <typename... Args>
    void construct(T *p, Args &&...args)
    {
        new (p) T(std::forward<Args>(args)...);
    }

    void destroy(T *p)
    {

        p->~T();
    }
};
