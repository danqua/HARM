#pragma once

#include "Engine/RenderCore/RenderCoreTypes.h"

namespace Hx {

    class RenderDevice {
    public:
        explicit RenderDevice(const RenderDeviceDesc& desc);
        ~RenderDevice();

        RenderDevice(const RenderDevice&) = delete;
        RenderDevice& operator=(const RenderDevice&) = delete;

        // Buffer methods
        BufferHandle CreateBuffer(const BufferDesc& desc);
        void DestroyBuffer(BufferHandle buffer);
        void BindBuffer(BufferHandle buffer);
        void BindVertexBuffer(BufferHandle buffer, u32 bindingIndex, usize offset, usize stride);
        void UpdateBuffer(BufferHandle buffer, const void* data, usize sizeInBytes, usize offset = 0);
    
        // Shader methods
        ShaderHandle CreateShader(const ShaderDesc& desc);
        void DestroyShader(ShaderHandle shader);

        // Program methods
        ProgramHandle CreateProgram(const ProgramDesc& desc);
        void DestroyProgram(ProgramHandle program);
        void BindProgram(ProgramHandle program);
        void SetUniformInt(ProgramHandle program, const char* name, int value);
        void SetUniformFloat(ProgramHandle program, const char* name, float value);
        void SetUniformVec2(ProgramHandle program, const char* name, const float* value);
        void SetUniformVec3(ProgramHandle program, const char* name, const float* value);
        void SetUniformVec4(ProgramHandle program, const char* name, const float* value);
        void SetUniformMat4(ProgramHandle program, const char* name, const float* value);

        // Texture methods
        TextureHandle CreateTexture(const TextureDesc& desc);
        void DestroyTexture(TextureHandle texture);
        void BindTexture(TextureHandle texture, u32 slot = 0);
        void UpdateTexture(TextureHandle texture, const void* data);

        // Vertex layout methods
        VertexLayoutHandle CreateVertexLayout(const VertexLayoutDesc& desc);
        void DestroyVertexLayout(VertexLayoutHandle layout);

        // Pipeline methods
        PipelineHandle CreatePipeline(const PipelineDesc& desc);
        void DestroyPipeline(PipelineHandle pipeline);
        void BindPipeline(PipelineHandle pipeline);

        // Framebuffer methods
        FramebufferHandle CreateFramebuffer(const FramebufferDesc& desc);
        void DestroyFramebuffer(FramebufferHandle framebuffer);

        // Render pass methods
        void BeginRenderPass(const RenderPassDesc& desc);
        void EndRenderPass();

        // Draw methods
        void Draw(u32 vertexCount, u32 startVertex = 0);
        void DrawIndexed(u32 indexCount, u32 startIndex = 0, s32 baseVertex = 0);
        
    private:
        struct RenderDeviceImpl* Impl;
    };

}