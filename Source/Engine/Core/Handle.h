#pragma once

#include "Engine/Core/Types.h"

namespace Engine {

    template <typename Tag>
    struct Handle {
        u32 Index = 0;
        u32 Gen   = 0;
        explicit operator bool() const { return Index != 0; }
    };

}