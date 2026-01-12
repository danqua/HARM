#pragma once

#include "Engine/Core/Types.h"
#include "Engine/Core/Handle.h"
#include "Engine/Math/Vector4.h"

namespace Engine::RenderCore {
    
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
        BufferType Type;
        BufferUsage Usage;
        usize SizeInBytes;
        const void* InitialData;
    };

    enum class ShaderStage : u8 {
        Vertex,
        Fragment,
        Compute
    };

    struct ShaderDesc {
        ShaderStage Stage;
        const char* Source;
        const char* DebugName;
    };

    struct ProgramDesc {
        ShaderHandle VertexShader;
        ShaderHandle FragmentShader;
        const char* DebugName;
    };

    enum class TextureFormat : u8 {
        RGBA8,
        RGB8,
        D24S8
    };

    struct TextureDesc {
        TextureFormat Format;
        u32 Width;
        u32 Height;
        const void* InitialData;
    };

    enum class VertexAttribFormat : u8 {
        Float,
        Float2,
        Float3,
        Float4,
        // Add more types as needed
    };

    struct VertexBinding {
        u32 Stride;
        u32 Divisor;
    };

    struct VertexAttributeDesc {
        u32 Location;
        u32 Binding;
        VertexAttribFormat Format;
        u32 Offset;
    };

    struct VertexLayoutDesc {
        VertexAttributeDesc Attributes[16];
        u32 AttributeCount;
        VertexBinding Bindings[8];
        u32 BindingCount;
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
        ProgramHandle Program;
        VertexLayoutHandle VertexLayout;

        PrimitiveTopology Topology = PrimitiveTopology::Triangles;

        bool DepthTest = true;
        bool DepthWrite = true;
        CompareOp DepthCompare = CompareOp::Less;

        FrontFace Front = FrontFace::CounterClockwise;
        CullMode Cull = CullMode::Back;
        bool Wireframe = false;
    };

    struct FramebufferDesc {
        u32 Width;
        u32 Height;
        TextureHandle ColorAttachments[4];
        TextureHandle DepthAttachment;
    };

    struct RenderPassDesc {
        FramebufferHandle Framebuffer;
        bool ClearColor;
        bool ClearDepth;
        Math::Vector4 ClearColorValue; // TODO: I should implement some color type
        float ClearDepthValue;
    };

    struct RenderDeviceDesc {
        u32 Width;
        u32 Height;
        bool VSync;
        bool DebugLayer;
    };
    
}