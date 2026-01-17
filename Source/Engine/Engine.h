#pragma once
#include "Engine/Core/Types.h"
#include "Engine/IO/FileSystem.h"
#include "Engine/Memory/ArenaAllocator.h"
#include "Engine/Math/Math.h"

namespace Engine {

    struct Context {
        Memory::ArenaAllocator* MainArena;
        Memory::ArenaAllocator* TransientArena;
        IO::FileSystem* FileSystem;
    };
    
    using Vec2 = Math::Vector2;
    using Vec3 = Math::Vector3;
    using Vec4 = Math::Vector4;
    using Mat4 = Math::Matrix4;

}