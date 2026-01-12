#pragma once

#include "Engine/Core/Types.h"

namespace Engine::Memory {

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

    using AllocFn   = void* (*)(Allocator* Self, usize Size, usize Alignment, AllocFlags Flags);
    using FreeFn    = void  (*)(Allocator* Self, void* Ptr, usize Size, usize Alignment);
    using ReallocFn = void* (*)(Allocator* Self, void* Ptr, usize OldSize, usize NewSize, usize Alignment, AllocFlags Flags);

    struct Allocator {
        AllocFn    Alloc;
        FreeFn     Free;
        ReallocFn  Realloc;

        AllocStats Stats;
        void*      UserData;
    };

    inline void* Alloc(Allocator* A, usize Size, usize Alignment, AllocFlags Flags = AllocFlags::None) {
        return A->Alloc(A, Size, Alignment, Flags);
    }

    inline void Free(Allocator* A, void* Ptr, usize Size, usize Alignment) {
        A->Free(A, Ptr, Size, Alignment);
    }

    inline void* Realloc(Allocator* A, void* Ptr, usize OldSize, usize NewSize, usize Alignment, AllocFlags Flags = AllocFlags::None) {
        if (!A->Realloc) {
            // Not all allocators implement Realloc
            return nullptr;
        }

        return A->Realloc(A, Ptr, OldSize, NewSize, Alignment, Flags);
    }

    // Convenience templates
    template <typename T>
    inline T* AllocOne(Allocator* A, AllocFlags Flags = AllocFlags::None) {
        return static_cast<T*>(Alloc(A, sizeof(T), alignof(T), Flags));
    }

    template <typename T>
    inline T* AllocArray(Allocator* A, usize Count, AllocFlags Flags = AllocFlags::None) {
        return static_cast<T*>(Alloc(A, sizeof(T) * Count, alignof(T), Flags));
    }

    template <typename T>
    inline void FreeArray(Allocator* A, T* Ptr, usize Count) {
        Free(A, static_cast<void*>(Ptr), sizeof(T) * Count, alignof(T));
    }

}