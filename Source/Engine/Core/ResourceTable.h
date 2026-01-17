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

        void Reserve(u32 Capacity) {
            // Index 0 is reserved, so we start from 1
            Records.reserve(Capacity + 1);
            Generations.reserve(Capacity + 1);
        }

        template<typename InitFn>
        Handle<Tag> Create(InitFn&& Init) {
            u32 Index = 0;
            if (!FreeIndices.empty()) {
                Index = FreeIndices.back();
                FreeIndices.pop_back();
            } else {
                Index = static_cast<u32>(Records.size());
                if (Index == 0) {
                    Index = 1;
                    // Add a dummy record at index 0
                    Records.emplace_back();
                    Generations.emplace_back(0);
                }

                Records.emplace_back();
                Generations.emplace_back(1);
            }

            Init(Records[Index]);

            return Handle<Tag>{ Index, Generations[Index] };
        }

        bool IsValid(Handle<Tag> H) const {
            if (H.Index == 0) return false;
            if (H.Index >= Generations.size()) return false;
            return Generations[H.Index] == H.Gen;
        }

        Record* TryGet(Handle<Tag> H) {
            return IsValid(H) ? &Records[H.Index] : nullptr;
        }

        const Record* TryGet(Handle<Tag> H) const {
            return IsValid(H) ? &Records[H.Index] : nullptr;
        }

        template <typename DestroyFn>
        bool Destroy(Handle<Tag> H, DestroyFn&& DestroyRecord) {
            if (!IsValid(H)) return false;

            Record& R = Records[H.Index];
            DestroyRecord(R);

            // Invalidate all existing handles
            ++Generations[H.Index];

            // Recycle slot
            FreeIndices.push_back(H.Index);

            return true;
        }

    private:

        std::vector<Record> Records;
        std::vector<u32>    Generations;
        std::vector<u32>    FreeIndices;
    };

}
