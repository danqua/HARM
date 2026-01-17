#pragma once

#include "Engine/Core/Types.h"
#include "Engine/Core/Handle.h"
#include "Engine/RenderCore/RenderDevice.h"
#include "Engine/Math/Math.h"

#include <vector>

namespace Hx {

    struct MeshTag {};
    struct StaticMeshTag {};
    struct MaterialTag {};

    using MeshHandle = Handle<MeshTag>;
    using StaticMeshHandle = Handle<StaticMeshTag>;
    using MaterialHandle = Handle<MaterialTag>;

    enum class MaterialType : u8 {
        Opaque,
        Transparent,
        Unlit
    };

    struct Vertex {
        Hx::Vector3 position;
        Hx::Vector3 normal;
        Hx::Vector2 texCoord;
    };

    class RenderSystem {
    public:
        RenderSystem(Hx::RenderDevice* inDevice);
        ~RenderSystem();

        void BeginFrame(const Hx::Matrix4& viewMatrix, const Hx::Matrix4& projectionMatrix);
        void EndFrame();

        MeshHandle CreateMesh(const Vertex* vertices, usize vertexCount, const u32* indices, usize indexCount);
        void DestroyMesh(MeshHandle mesh);

        MaterialHandle CreateMaterial(MaterialType type);
        void DestroyMaterial(MaterialHandle material);

        void Submit(MeshHandle mesh, MaterialHandle material, const Hx::Matrix4& transform);

    private:
        
        void FlushDrawCommands();

    private:

        struct RenderSystemImpl* Impl;
    };

}