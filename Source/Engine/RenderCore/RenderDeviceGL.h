#pragma once

#include "Engine/Core/ResourceTable.h"
#include "Engine/RenderCore/RenderDevice.h"

namespace Engine::RenderCore {

    constexpr usize MaxBuffers       = 128;
    constexpr usize MaxShaders       = 64;
    constexpr usize MaxPrograms      = 64;
    constexpr usize MaxTextures      = 128;
    constexpr usize MaxVertexLayouts = 16;
    constexpr usize MaxPipelines     = 64;
    constexpr usize MaxFramebuffers  = 16;

    struct GLBuffer {
        u32 Id;
        BufferType Type;
        BufferUsage Usage;
        usize SizeInBytes;
    };

    struct GLShader {
        u32 Id;
    };

    struct GLProgram {
        u32 Id;
    };

    struct GLTexture {
        u32 Id;
        u32 Width;
        u32 Height;
        TextureFormat Format;
    };

    struct VertexLayout {
        u32 Vao;
    };

    struct GLPipeline {
        ProgramHandle Program;
        PrimitiveTopology Topology;
        VertexLayoutHandle VertexLayout;

        bool DepthTest;
        bool DepthWrite;
        CompareOp DepthCompare;

        FrontFace Front;
        CullMode Cull;

        bool Wireframe;
    };

    struct GLFramebuffer {
        u32 Id;
        u32 Width;
        u32 Height;
        TextureHandle ColorAttachments[4];
        u32 ColorAttachmentCount;
        TextureHandle DepthAttachment;
    };

    struct RenderDeviceImpl {
        ResourceTable<BufferTag, GLBuffer> Buffers;
        ResourceTable<ShaderTag, GLShader> Shaders;
        ResourceTable<ProgramTag, GLProgram> Programs;
        ResourceTable<TextureTag, GLTexture> Textures;
        ResourceTable<VertexLayoutTag, VertexLayout> VertexLayouts;
        ResourceTable<PipelineTag, GLPipeline> Pipelines;
        ResourceTable<FramebufferTag, GLFramebuffer> Framebuffers;

        PrimitiveTopology CurrentTopology = PrimitiveTopology::Triangles;
        u32 CurrentProgram = 0;

        u32 DefaultFramebufferWidth = 800;
        u32 DefaultFramebufferHeight = 600;
    };

}