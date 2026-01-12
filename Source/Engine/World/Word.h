#pragma once

#include "Engine/Core/Handle.h"

namespace Engine::World {

    struct EntityTag {};

    using EntityHandle = Handle<EntityTag>;

    class World {
    public:
        World() = default;
        ~World() = default;
    };

};