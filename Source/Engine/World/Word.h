#pragma once

#include "Engine/Core/Handle.h"

namespace Hx {

    struct EntityTag {};

    using EntityHandle = Handle<EntityTag>;

    class World {
    public:
        World() = default;
        ~World() = default;
    };

};