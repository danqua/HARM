#pragma once

#include "Allocator.h"
#include <cassert>
#include <cstring>

namespace Engine::Memory {

    struct ArenaAllocator {
        Allocator Base;
        void*     Begin;
        void*     End;
        void*     Current;
    };

    inline uintptr_t AlignForward(uintptr_t Ptr, usize Alignment) {
        const uintptr_t Mask = Alignment - 1;
        return (Ptr + Mask) & ~Mask;
    }

    inline void* ArenaAlloc(Allocator* Self, usize Size, usize Alignment, AllocFlags Flags) {
        ArenaAllocator* Arena = static_cast<ArenaAllocator*>(Self->UserData);
        assert(Arena);

        uintptr_t CurrentAddress = reinterpret_cast<uintptr_t>(Arena->Current);
        uintptr_t AlignedAddress = AlignForward(CurrentAddress, Alignment);
        uintptr_t NewAddress = AlignedAddress + Size;

        if (NewAddress > reinterpret_cast<uintptr_t>(Arena->End)) {
            if (HasFlag(Flags, AllocFlags::NoFail)) {
                assert(false && "ArenaAllocator is out of memory!");
            }

            return nullptr;
        }

        Arena->Current = reinterpret_cast<void*>(NewAddress);
        void* Result = reinterpret_cast<void*>(AlignedAddress);

        if (HasFlag(Flags, AllocFlags::ZeroInit)) {
            std::memset(Result, 0, Size);
        }

        // Update stats
        Self->Stats.BytesInUse = reinterpret_cast<u64>(Arena->Current) - reinterpret_cast<u64>(Arena->Begin);
        Self->Stats.TotalAllocations++;

        return Result;
    }

    inline void ArenaFree(Allocator* Self, void* Ptr, usize Size, usize Alignment) {
        // Arenas do not support freeing individual allocations.
        // Memory is freed when the arena is reset or destroyed.
        (void)Self;
        (void)Ptr;
        (void)Size;
        (void)Alignment;
    }

    inline void InitArena(ArenaAllocator& Arena, void* Memory, usize Size) {
        Arena.Begin = Memory;
        Arena.End = static_cast<void*>(static_cast<u8*>(Memory) + Size);
        Arena.Current = Arena.Begin;

        Arena.Base.Alloc = ArenaAlloc;
        Arena.Base.Free = ArenaFree;
        Arena.Base.Realloc = nullptr;
        Arena.Base.Stats = AllocStats();
        Arena.Base.UserData = &Arena;
     }

    inline void ResetArena(ArenaAllocator& Arena) {
        Arena.Current = Arena.Begin;
        Arena.Base.Stats.BytesInUse = 0;
    }
}