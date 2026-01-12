#pragma once

#include "Engine/Core/Types.h"
#include "Engine/Core/Handle.h"
#include "Engine/RenderCore/RenderDevice.h"
#include "Engine/Math/Math.h"

#include <vector>

namespace Engine::Renderer {

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
        Math::Vector3 Position;
        Math::Vector3 Normal;
        Math::Vector2 TexCoord;
    };

    class RenderSystem {
    public:
        RenderSystem(RenderCore::RenderDevice* InDevice);
        ~RenderSystem();

        void BeginFrame(const Math::Matrix4& ViewMatrix, const Math::Matrix4& ProjectionMatrix);
        void EndFrame();

        MeshHandle CreateMesh(const Vertex* Vertices, usize VertexCount, const u32* Indices, usize IndexCount);
        void DestroyMesh(MeshHandle Mesh);

        MaterialHandle CreateMaterial(MaterialType Type);
        void DestroyMaterial(MaterialHandle Material);

        void Submit(MeshHandle Mesh, MaterialHandle Material, const Math::Matrix4& Transform);

    private:
        
        void FlushDrawCommands();

    private:

        struct RenderSystemImpl* Impl;
    };

}