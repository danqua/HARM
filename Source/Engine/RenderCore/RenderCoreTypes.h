#pragma once

#include "Engine/Core/Types.h"
#include "Engine/Core/Handle.h"
#include "Engine/Math/Vector4.h"

namespace Hx {
    
    struct BufferTag {};
    struct ShaderTag {};
    struct ProgramTag {};
    struct TextureTag {};
    struct VertexLayoutTag {};
    struct PipelineTag {};
    struct FramebufferTag {};

    using BufferHandle = Handle<BufferTag>;
    using ShaderHandle = Handle<ShaderTag>;
    using ProgramHandle = Handle<ProgramTag>;
    using TextureHandle = Handle<TextureTag>;
    using VertexLayoutHandle = Handle<VertexLayoutTag>;
    using PipelineHandle = Handle<PipelineTag>;
    using FramebufferHandle = Handle<FramebufferTag>;

    enum class BufferType : u8 {
        Vertex,
        Index,
        Uniform
    };

    enum class BufferUsage : u8 {
        Static,
        Dynamic,
        Stream
    };

    struct BufferDesc {
        BufferType type;
        BufferUsage usage;
        usize sizeInBytes;
        const void* initialData;
    };

    enum class ShaderStage : u8 {
        Vertex,
        Fragment,
        Compute
    };

    struct ShaderDesc {
        ShaderStage stage;
        const char* source;
        const char* debugName;
    };

    struct ProgramDesc {
        ShaderHandle vertexShader;
        ShaderHandle fragmentShader;
        const char* debugName;
    };

    enum class TextureFormat : u8 {
        RGBA8,
        RGB8,
        D24S8
    };

    struct TextureDesc {
        TextureFormat format;
        u32 width;
        u32 height;
        const void* initialData;
    };

    enum class VertexAttribFormat : u8 {
        Float,
        Float2,
        Float3,
        Float4,
        // Add more types as needed
    };

    struct VertexBinding {
        u32 stride;
        u32 divisor;
    };

    struct VertexAttributeDesc {
        u32 location;
        u32 binding;
        VertexAttribFormat format;
        u32 offset;
    };

    struct VertexLayoutDesc {
        VertexAttributeDesc attributes[16];
        u32 attributeCount;
        VertexBinding bindings[8];
        u32 bindingCount;
    };

    enum class CullMode : u8 {
        None,
        Front,
        Back
    };

    enum class FrontFace : u8 {
        Clockwise,
        CounterClockwise
    };

    enum class PrimitiveTopology : u8 {
        Triangles,
        Lines,
        Points
    };

    enum class CompareOp : u8 {
        Never,
        Less,
        Equal,
        LessEqual,
        Greater,
        NotEqual,
        GreaterEqual,
        Always
    };

    struct PipelineDesc {
        ProgramHandle program;
        VertexLayoutHandle vertexLayout;

        PrimitiveTopology topology = PrimitiveTopology::Triangles;

        bool depthTest = true;
        bool depthWrite = true;
        CompareOp depthCompare = CompareOp::Less;

        FrontFace front = FrontFace::CounterClockwise;
        CullMode cull = CullMode::Back;
        bool wireframe = false;
    };

    struct FramebufferDesc {
        u32 width;
        u32 height;
        TextureHandle colorAttachments[4];
        TextureHandle depthAttachment;
    };

    struct RenderPassDesc {
        FramebufferHandle framebuffer;
        bool clearColor;
        bool clearDepth;
        Hx::Vector4 clearColorValue; // TODO: I should implement some color type
        float clearDepthValue;
    };

    struct RenderDeviceDesc {
        u32 width;
        u32 height;
        bool vSync;
        bool debugLayer;
    };
    
}