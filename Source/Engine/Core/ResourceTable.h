#pragma once 

#include "Engine/Core/Handle.h"
#include "Engine/Core/Types.h"
#include <vector>
#include <utility>

namespace Hx {

    template <typename Tag, typename Record>
    class ResourceTable {
    public:

        ResourceTable() = default;

        void Reserve(u32 capacity) {
            // Index 0 is reserved, so we start from 1
            records.reserve(capacity + 1);
            generations.reserve(capacity + 1);
        }

        template<typename InitFn>
        Handle<Tag> Create(InitFn&& initFn) {
            u32 index = 0;
            if (!freeIndices.empty()) {
                index = freeIndices.back();
                freeIndices.pop_back();
            } else {
                index = static_cast<u32>(records.size());
                if (index == 0) {
                    index = 1;
                    // Add a dummy record at index 0
                    records.emplace_back();
                    generations.emplace_back(0);
                }

                records.emplace_back();
                generations.emplace_back(1);
            }

            initFn(records[index]);

            return Handle<Tag>{ index, generations[index] };
        }

        bool IsValid(Handle<Tag> h) const {
            if (h.index == 0) return false;
            if (h.index >= generations.size()) return false;
            return generations[h.index] == h.gen;
        }

        Record* TryGet(Handle<Tag> h) {
            return IsValid(h) ? &records[h.index] : nullptr;
        }

        const Record* TryGet(Handle<Tag> h) const {
            return IsValid(h) ? &records[h.index] : nullptr;
        }

        template <typename DestroyFn>
        bool Destroy(Handle<Tag> h, DestroyFn&& destroyRecord) {
            if (!IsValid(h)) return false;

            Record& r = records[h.index];
            destroyRecord(r);

            // Invalidate all existing handles
            ++generations[h.index];

            // Recycle slot
            freeIndices.push_back(h.index);

            return true;
        }

    private:

        std::vector<Record> records;
        std::vector<u32>    generations;
        std::vector<u32>    freeIndices;
    };

}
