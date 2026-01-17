#include "Engine/Renderer/RenderSystem.h"
#include "Engine/Core/ResourceTable.h"
#include "Engine/IO/FileSystem.h"

namespace Hx {

    struct MeshRecord {
        MeshHandle handle;
        Hx::BufferHandle vertexBuffer;
        Hx::BufferHandle indexBuffer;
        usize indexCount;
    };

    struct StaticMeshRecord {
        StaticMeshHandle handle;
        
    };

    struct MaterialRecord {
        MaterialHandle handle;
        Hx::ProgramHandle program;
        Hx::PipelineHandle pipeline;
        Hx::Vector4 diffuseColor;
    };

    constexpr usize MaxDrawCommands = 1024;

    struct DrawCommand {
        MaterialHandle material;
        Hx::PipelineHandle pipeline;
        Hx::BufferHandle vertexBuffer;
        Hx::BufferHandle indexBuffer;
        Hx::Matrix4 transform;
        u32 indexCount;
        u32 indexOffset;
        u32 vertexOffset;
    };
    
    struct RenderSystemImpl {
        Hx::RenderDevice* device;

        ResourceTable<MeshTag, MeshRecord> meshTable;
        ResourceTable<StaticMeshTag, StaticMeshRecord> staticMeshTable;
        ResourceTable<MaterialTag, MaterialRecord> materialTable;

        DrawCommand drawCommands[MaxDrawCommands];
        usize drawCommandCount = 0;

        Hx::Matrix4 currentViewMatrix;
        Hx::Matrix4 currentProjectionMatrix;

        Hx::PipelineHandle opaquePipeline;
        Hx::PipelineHandle transparentPipeline;
        Hx::PipelineHandle unlitPipeline;

        Hx::ProgramHandle opaqueShaderProgram;
        Hx::ProgramHandle transparentShaderProgram;
        Hx::ProgramHandle unlitShaderProgram;
    };

    inline static Hx::ShaderHandle LoadShaderFromFile(Hx::RenderDevice* device, const char* filename, Hx::ShaderStage stage) {
        Hx::FileSystem fileSystem;

        Hx::FileHandle* handle = fileSystem.OpenFileRead(filename);
        if (!handle) return Hx::ShaderHandle{};

        usize fileSize = handle->GetSize();
        char* buffer = new char[fileSize + 1]{};

        handle->Read(buffer, fileSize);

        Hx::ShaderDesc shaderDesc = {};
        shaderDesc.stage = stage;
        shaderDesc.source = buffer;
        shaderDesc.debugName = filename;

        Hx::ShaderHandle shader = device->CreateShader(shaderDesc);

        fileSystem.CloseFile(handle);

        return shader;
    }

    inline static Hx::ProgramHandle CreateShaderProgram(Hx::RenderDevice* device, const char* vertexShaderPath, const char* fragmentShaderPath, const char* debugName) {
        Hx::ShaderHandle vertexShader = LoadShaderFromFile(device, vertexShaderPath, Hx::ShaderStage::Vertex);
        if (!vertexShader) {
            return Hx::ProgramHandle{};
        }

        Hx::ShaderHandle fragmentShader = LoadShaderFromFile(device, fragmentShaderPath, Hx::ShaderStage::Fragment);
        if (!fragmentShader) {
            device->DestroyShader(vertexShader);
            return Hx::ProgramHandle{};
        }

        Hx::ProgramDesc programDesc = {};
        programDesc.vertexShader = vertexShader;
        programDesc.fragmentShader = fragmentShader;
        programDesc.debugName = debugName;

        Hx::ProgramHandle program = device->CreateProgram(programDesc);

        device->DestroyShader(vertexShader);
        device->DestroyShader(fragmentShader);

        return program;
    }

    static inline Hx::PipelineHandle CreatePipeline(Hx::RenderDevice* device, Hx::ProgramHandle program, MaterialType type) {
        Hx::VertexLayoutDesc vertexLayoutDesc;
        // Configure pipeline based on material type
        switch (type) {
            case MaterialType::Opaque:
            case MaterialType::Transparent: {
                vertexLayoutDesc.attributeCount = 3;
                vertexLayoutDesc.attributes[0] = { 0, 0, Hx::VertexAttribFormat::Float3, 0 };   // Position
                vertexLayoutDesc.attributes[1] = { 1, 0, Hx::VertexAttribFormat::Float3, 12 };  // Normal
                vertexLayoutDesc.attributes[2] = { 2, 0, Hx::VertexAttribFormat::Float2, 24 };  // TexCoord
                vertexLayoutDesc.bindingCount = 1;
                vertexLayoutDesc.bindings[0] = { 32, 0 }; // Stride, Divisor
            } break;
            case MaterialType::Unlit: {
                vertexLayoutDesc.attributeCount = 2;
                vertexLayoutDesc.attributes[0] = { 0, 0, Hx::VertexAttribFormat::Float3, 0 };   // Position
                vertexLayoutDesc.attributes[1] = { 1, 0, Hx::VertexAttribFormat::Float2, 12 };  // TexCoord
                vertexLayoutDesc.bindingCount = 1;
                vertexLayoutDesc.bindings[0] = { 20, 0 }; // Stride, Divisor
            } break;
        }

        Hx::PipelineDesc pipelineDesc;
        pipelineDesc.program = program;
        pipelineDesc.vertexLayout = device->CreateVertexLayout(vertexLayoutDesc);

        Hx::PipelineHandle pipeline = device->CreatePipeline(pipelineDesc);
        return pipeline;
    }

    RenderSystem::RenderSystem(Hx::RenderDevice* inDevice) {
        Impl = new RenderSystemImpl();
        Impl->device = inDevice;

        Impl->opaqueShaderProgram = CreateShaderProgram(Impl->device, "Shaders/Opaque.vert", "Shaders/Opaque.frag", "OpaqueShaderProgram");
        Impl->transparentShaderProgram = CreateShaderProgram(Impl->device, "Shaders/Transparent.vert", "Shaders/Transparent.frag", "TransparentShaderProgram");
        Impl->unlitShaderProgram = CreateShaderProgram(Impl->device, "Shaders/Unlit.vert", "Shaders/Unlit.frag", "UnlitShaderProgram");
        
        Impl->opaquePipeline = CreatePipeline(Impl->device, Impl->opaqueShaderProgram, MaterialType::Opaque);
        Impl->transparentPipeline = CreatePipeline(Impl->device, Impl->transparentShaderProgram, MaterialType::Transparent);
        Impl->unlitPipeline = CreatePipeline(Impl->device, Impl->unlitShaderProgram, MaterialType::Unlit);
    }

    RenderSystem::~RenderSystem() {

    }

    void RenderSystem::BeginFrame(const Hx::Matrix4& viewMatrix, const Hx::Matrix4& projectionMatrix) {
        Impl->currentViewMatrix = viewMatrix;
        Impl->currentProjectionMatrix = projectionMatrix;
    }

    void RenderSystem::EndFrame() {
        FlushDrawCommands();
        Impl->drawCommandCount = 0;
    }

    MeshHandle RenderSystem::CreateMesh(const Vertex* vertices, usize vertexCount, const u32* indices, usize indexCount) {
        return Impl->meshTable.Create([&](MeshRecord& mesh) {
            Hx::BufferDesc vertexBufferDesc;
            vertexBufferDesc.type = Hx::BufferType::Vertex;
            vertexBufferDesc.usage = Hx::BufferUsage::Static;
            vertexBufferDesc.sizeInBytes = vertexCount * sizeof(Vertex);
            vertexBufferDesc.initialData = vertices;

            Hx::BufferDesc indexBufferDesc;
            indexBufferDesc.type = Hx::BufferType::Index;
            indexBufferDesc.usage = Hx::BufferUsage::Static;
            indexBufferDesc.sizeInBytes = indexCount * sizeof(u32);
            indexBufferDesc.initialData = indices;

            mesh.handle = mesh.handle;
            mesh.vertexBuffer = Impl->device->CreateBuffer(vertexBufferDesc);
            mesh.indexBuffer = Impl->device->CreateBuffer(indexBufferDesc);
            mesh.indexCount = indexCount;
        });
    }

    void RenderSystem::DestroyMesh(MeshHandle mesh) {
        Impl->meshTable.Destroy(mesh, [&](MeshRecord& meshRecord) {
            Impl->device->DestroyBuffer(meshRecord.vertexBuffer);
            Impl->device->DestroyBuffer(meshRecord.indexBuffer);
        });
    }

    MaterialHandle RenderSystem::CreateMaterial(MaterialType type) {
        return Impl->materialTable.Create([&](MaterialRecord& material) {
            material.handle = material.handle;

            switch (type) {
                case MaterialType::Opaque:
                    material.program = Impl->opaqueShaderProgram;
                    material.pipeline = Impl->opaquePipeline;
                    material.diffuseColor = Hx::Vector4(1.0f, 1.0f, 1.0f, 1.0f);
                    break;
                case MaterialType::Transparent:
                    material.program = Impl->transparentShaderProgram;
                    material.pipeline = Impl->transparentPipeline;
                    material.diffuseColor = Hx::Vector4(1.0f, 1.0f, 1.0f, 0.5f);
                    break;
                case MaterialType::Unlit:
                    material.program = Impl->unlitShaderProgram;
                    material.pipeline = Impl->unlitPipeline;
                    material.diffuseColor = Hx::Vector4(1.0f, 1.0f, 1.0f, 1.0f);
                    break;
            }

        });
    }

    void RenderSystem::DestroyMaterial(MaterialHandle material) {
        Impl->materialTable.Destroy(material, [&](MaterialRecord& materialRecord) {
            // Note: Shader programs and pipelines are shared; do not destroy here
        });
    }
    
    void RenderSystem::Submit(MeshHandle mesh, MaterialHandle material, const Hx::Matrix4& transform) {
        if (Impl->drawCommandCount >= MaxDrawCommands) {
            FlushDrawCommands();
        }

        DrawCommand& cmd = Impl->drawCommands[Impl->drawCommandCount++];
        cmd.material = material;
        cmd.pipeline = Impl->materialTable.TryGet(material)->pipeline;
        cmd.vertexBuffer = Impl->meshTable.TryGet(mesh)->vertexBuffer;
        cmd.indexBuffer = Impl->meshTable.TryGet(mesh)->indexBuffer;
        cmd.transform = transform;
        cmd.indexCount = static_cast<u32>(Impl->meshTable.TryGet(mesh)->indexCount);
        cmd.indexOffset = 0;
        cmd.vertexOffset = 0;
    }

    void RenderSystem::FlushDrawCommands() {
        Hx::RenderDevice* device = Impl->device;

        Hx::RenderPassDesc opaquePassDesc = {};
        opaquePassDesc.clearColor = true;
        opaquePassDesc.clearDepth = true;
        opaquePassDesc.clearColorValue = Hx::Vector4(0.1f, 0.1f, 0.1f, 1.0f);
        opaquePassDesc.clearDepthValue = 1.0f;

        device->BeginRenderPass(opaquePassDesc);

        for (usize i = 0; i < Impl->drawCommandCount; ++i) {
            const DrawCommand& cmd = Impl->drawCommands[i];

            const MaterialRecord* material = Impl->materialTable.TryGet(cmd.material);
            if (!material) continue;

            device->BindPipeline(cmd.pipeline);
            device->SetUniformMat4(material->program, "uModelMatrix", cmd.transform.m);
            device->SetUniformMat4(material->program, "uViewMatrix", Impl->currentViewMatrix.m);
            device->SetUniformMat4(material->program, "uProjectionMatrix", Impl->currentProjectionMatrix.m);
            device->SetUniformVec4(material->program, "uDiffuseColor", &material->diffuseColor.x);

            device->BindVertexBuffer(cmd.vertexBuffer, 0, 0, sizeof(Vertex));
            device->BindBuffer(cmd.indexBuffer);

            device->DrawIndexed(cmd.indexCount, cmd.indexOffset, cmd.vertexOffset);
        }

        Impl->drawCommandCount = 0;
        device->EndRenderPass();
    }
    
}
