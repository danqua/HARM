#pragma once

#include "Allocator.h"
#include <cassert>
#include <cstring>

namespace Hx {

    struct ArenaAllocator {
        Allocator base;
        void*     begin;
        void*     end;
        void*     current;
    };;

    inline uintptr_t AlignForward(uintptr_t ptr, usize alignment) {
        const uintptr_t mask = alignment - 1;
        return (ptr + mask) & ~mask;
    }

    inline void* ArenaAlloc(Allocator* self, usize size, usize alignment, AllocFlags flags) {
        ArenaAllocator* arena = static_cast<ArenaAllocator*>(self->userData);
        assert(arena);

        uintptr_t currentAddress = reinterpret_cast<uintptr_t>(arena->current);
        uintptr_t alignedAddress = AlignForward(currentAddress, alignment);
        uintptr_t newAddress = alignedAddress + size;

        if (newAddress > reinterpret_cast<uintptr_t>(arena->end)) {
            if (HasFlag(flags, AllocFlags::NoFail)) {
                assert(false && "ArenaAllocator is out of memory!");
            }

            return nullptr;
        }

        arena->current = reinterpret_cast<void*>(newAddress);
        void* result = reinterpret_cast<void*>(alignedAddress);

        if (HasFlag(flags, AllocFlags::ZeroInit)) {
            std::memset(result, 0, size);
        }

        // Update stats
        self->stats.BytesInUse = reinterpret_cast<u64>(arena->current) - reinterpret_cast<u64>(arena->begin);
        self->stats.TotalAllocations++;

        return result;
    }

    inline void ArenaFree(Allocator* self, void* ptr, usize size, usize alignment) {
        // Arenas do not support freeing individual allocations.
        // Memory is freed when the arena is reset or destroyed.
        (void)self;
        (void)ptr;
        (void)size;
        (void)alignment;
    }

    inline void InitArena(ArenaAllocator& arena, void* memory, usize size) {
        arena.begin = memory;
        arena.end = static_cast<void*>(static_cast<u8*>(memory) + size);
        arena.current = arena.begin;

        arena.base.alloc = ArenaAlloc;
        arena.base.free = ArenaFree;
        arena.base.realloc = nullptr;
        arena.base.stats = AllocStats();
        arena.base.userData = &arena;
     }

    inline void ResetArena(ArenaAllocator& arena) {
        arena.current = arena.begin;
        arena.base.stats.BytesInUse = 0;
    }
}