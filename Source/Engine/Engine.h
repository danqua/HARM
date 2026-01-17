#pragma once
#include "Engine/Core/Types.h"
#include "Engine/IO/FileSystem.h"
#include "Engine/Memory/ArenaAllocator.h"
#include "Engine/Math/Math.h"

namespace Hx {

    struct Context {
        ArenaAllocator* mainArena;
        ArenaAllocator* transientArena;
        FileSystem* fileSystem;
    };

}