#pragma once

#include "Engine/Core/Types.h"

namespace Hx {

    struct AllocStats {
        u64 BytesInUse = 0;
        u64 TotalAllocations = 0;
        u64 TotalFrees = 0;
    };

    enum class AllocFlags : u32 {
        None     = 0,
        NoFail   = 1 << 0,
        ZeroInit = 1 << 1
    };

    constexpr bool HasFlag(AllocFlags Flags, AllocFlags FlagsToCheck) {
        return (static_cast<u32>(Flags) & static_cast<u32>(FlagsToCheck)) != 0;
    }

    constexpr usize Kilobytes(usize Kb) {
        return Kb * 1024;
    }

    constexpr usize Megabytes(usize Mb) {
        return Mb * 1024 * 1024;
    }

    constexpr usize Gigabytes(usize Gb) {
        return Gb * 1024 * 1024 * 1024;
    }

    constexpr usize DefaultAlignment = alignof(std::max_align_t);

    struct Allocator;

    using AllocFn   = void* (*)(Allocator* self, usize size, usize alignment, AllocFlags flags);
    using FreeFn    = void  (*)(Allocator* self, void* ptr, usize size, usize alignment);
    using ReallocFn = void* (*)(Allocator* self, void* ptr, usize oldSize, usize newSize, usize alignment, AllocFlags flags);

    struct Allocator {
        AllocFn    alloc;
        FreeFn     free;
        ReallocFn  realloc;

        AllocStats stats;
        void*      userData;
    };;

    inline void* Alloc(Allocator* a, usize size, usize alignment, AllocFlags flags = AllocFlags::None) {
        return a->alloc(a, size, alignment, flags);
    }

    inline void Free(Allocator* a, void* ptr, usize size, usize alignment) {
        a->free(a, ptr, size, alignment);
    }

    inline void* Realloc(Allocator* a, void* ptr, usize oldSize, usize newSize, usize alignment, AllocFlags flags = AllocFlags::None) {
        if (!a->realloc) {
            // Not all allocators implement Realloc
            return nullptr;
        }

        return a->realloc(a, ptr, oldSize, newSize, alignment, flags);
    }

    // Convenience templates
    template <typename T>
    inline T* AllocOne(Allocator* a, AllocFlags flags = AllocFlags::None) {
        return static_cast<T*>(Alloc(a, sizeof(T), alignof(T), flags));
    }

    template <typename T>
    inline T* AllocArray(Allocator* a, usize count, AllocFlags flags = AllocFlags::None) {
        return static_cast<T*>(Alloc(a, sizeof(T) * count, alignof(T), flags));
    }

    template <typename T>
    inline void FreeArray(Allocator* a, T* ptr, usize count) {
        Free(a, static_cast<void*>(ptr), sizeof(T) * count, alignof(T));
    }

}