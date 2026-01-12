#pragma once

#include "Engine/RenderCore/RenderCoreTypes.h"

namespace Engine::RenderCore {

    class RenderDevice {
    public:
        explicit RenderDevice(const RenderDeviceDesc& Desc);
        ~RenderDevice();

        RenderDevice(const RenderDevice&) = delete;
        RenderDevice& operator=(const RenderDevice&) = delete;

        // Buffer methods
        BufferHandle CreateBuffer(const BufferDesc& Desc);
        void DestroyBuffer(BufferHandle Buffer);
        void BindBuffer(BufferHandle Buffer);
        void BindVertexBuffer(BufferHandle Buffer, u32 BindingIndex, usize Offset, usize Stride);
        void UpdateBuffer(BufferHandle Buffer, const void* Data, usize SizeInBytes, usize Offset = 0);
    
        // Shader methods
        ShaderHandle CreateShader(const ShaderDesc& Desc);
        void DestroyShader(ShaderHandle Shader);

        // Program methods
        ProgramHandle CreateProgram(const ProgramDesc& Desc);
        void DestroyProgram(ProgramHandle Program);
        void BindProgram(ProgramHandle Program);
        void SetUniformInt(ProgramHandle Program, const char* Name, int Value);
        void SetUniformFloat(ProgramHandle Program, const char* Name, float Value);
        void SetUniformVec2(ProgramHandle Program, const char* Name, const float* Value);
        void SetUniformVec3(ProgramHandle Program, const char* Name, const float* Value);
        void SetUniformVec4(ProgramHandle Program, const char* Name, const float* Value);
        void SetUniformMat4(ProgramHandle Program, const char* Name, const float* Value);

        // Texture methods
        TextureHandle CreateTexture(const TextureDesc& Desc);
        void DestroyTexture(TextureHandle Texture);
        void BindTexture(TextureHandle Texture, u32 Slot = 0);
        void UpdateTexture(TextureHandle Texture, const void* Data);

        // Vertex layout methods
        VertexLayoutHandle CreateVertexLayout(const VertexLayoutDesc& Desc);
        void DestroyVertexLayout(VertexLayoutHandle Layout);

        // Pipeline methods
        PipelineHandle CreatePipeline(const PipelineDesc& Desc);
        void DestroyPipeline(PipelineHandle Pipeline);
        void BindPipeline(PipelineHandle Pipeline);

        // Framebuffer methods
        FramebufferHandle CreateFramebuffer(const FramebufferDesc& Desc);
        void DestroyFramebuffer(FramebufferHandle Framebuffer);

        // Render pass methods
        void BeginRenderPass(const RenderPassDesc& Desc);
        void EndRenderPass();

        // Draw methods
        void Draw(u32 VertexCount, u32 StartVertex = 0);
        void DrawIndexed(u32 IndexCount, u32 StartIndex = 0, s32 BaseVertex = 0);
        
    private:
        struct RenderDeviceImpl* Impl;
    };

}