#pragma once

#include "Engine/Core/ResourceTable.h"
#include "Engine/RenderCore/RenderDevice.h"

namespace Hx {

    constexpr usize MaxBuffers       = 128;
    constexpr usize MaxShaders       = 64;
    constexpr usize MaxPrograms      = 64;
    constexpr usize MaxTextures      = 128;
    constexpr usize MaxVertexLayouts = 16;
    constexpr usize MaxPipelines     = 64;
    constexpr usize MaxFramebuffers  = 16;

    struct GLBuffer {
        u32 id;
        BufferType type;
        BufferUsage usage;
        usize sizeInBytes;
    };

    struct GLShader {
        u32 id;
    };

    struct GLProgram {
        u32 id;
    };

    struct GLTexture {
        u32 id;
        u32 width;
        u32 height;
        TextureFormat format;
    };

    struct VertexLayout {
        u32 vao;
    };

    struct GLPipeline {
        ProgramHandle program;
        PrimitiveTopology topology;
        VertexLayoutHandle vertexLayout;

        bool depthTest;
        bool depthWrite;
        CompareOp depthCompare;

        FrontFace front;
        CullMode cull;

        bool wireframe;
    };

    struct GLFramebuffer {
        u32 id;
        u32 width;
        u32 height;
        TextureHandle colorAttachments[4];
        u32 colorAttachmentCount;
        TextureHandle depthAttachment;
    };

    struct RenderDeviceImpl {
        ResourceTable<BufferTag, GLBuffer> buffers;
        ResourceTable<ShaderTag, GLShader> shaders;
        ResourceTable<ProgramTag, GLProgram> programs;
        ResourceTable<TextureTag, GLTexture> textures;
        ResourceTable<VertexLayoutTag, VertexLayout> vertexLayouts;
        ResourceTable<PipelineTag, GLPipeline> pipelines;
        ResourceTable<FramebufferTag, GLFramebuffer> framebuffers;

        PrimitiveTopology currentTopology = PrimitiveTopology::Triangles;
        u32 currentProgram = 0;

        u32 defaultFramebufferWidth = 800;
        u32 defaultFramebufferHeight = 600;
    };

}