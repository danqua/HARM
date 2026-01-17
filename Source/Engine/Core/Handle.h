#pragma once

#include "Engine/Core/Types.h"

namespace Hx {

    template <typename Tag>
    struct Handle {
        u32 index = 0;
        u32 gen   = 0;
        explicit operator bool() const { return index != 0; }
    };

}