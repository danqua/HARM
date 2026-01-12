#include "Engine/Renderer/RenderSystem.h"
#include "Engine/Core/ResourceTable.h"
#include "Engine/IO/FileSystem.h"

namespace Engine::Renderer {

    struct MeshRecord {
        MeshHandle Handle;
        RenderCore::BufferHandle VertexBuffer;
        RenderCore::BufferHandle IndexBuffer;
        usize IndexCount;
    };

    struct StaticMeshRecord {
        StaticMeshHandle Handle;
        
    };

    struct MaterialRecord {
        MaterialHandle Handle;
        RenderCore::ProgramHandle Program;
        RenderCore::PipelineHandle Pipeline;
        Math::Vector4 DiffuseColor;
    };

    constexpr usize MaxDrawCommands = 1024;

    struct DrawCommand {
        MaterialHandle Material;
        RenderCore::PipelineHandle Pipeline;
        RenderCore::BufferHandle VertexBuffer;
        RenderCore::BufferHandle IndexBuffer;
        Math::Matrix4 Transform;
        u32 IndexCount;
        u32 IndexOffset;
        u32 VertexOffset;
    };
    
    struct RenderSystemImpl {
        RenderCore::RenderDevice* Device;

        ResourceTable<MeshTag, MeshRecord> MeshTable;
        ResourceTable<StaticMeshTag, StaticMeshRecord> StaticMeshTable;
        ResourceTable<MaterialTag, MaterialRecord> MaterialTable;

        DrawCommand DrawCommands[MaxDrawCommands];
        usize DrawCommandCount = 0;

        Math::Matrix4 CurrentViewMatrix;
        Math::Matrix4 CurrentProjectionMatrix;

        RenderCore::PipelineHandle OpaquePipeline;
        RenderCore::PipelineHandle TransparentPipeline;
        RenderCore::PipelineHandle UnlitPipeline;

        RenderCore::ProgramHandle OpaqueShaderProgram;
        RenderCore::ProgramHandle TransparentShaderProgram;
        RenderCore::ProgramHandle UnlitShaderProgram;
    };

    inline static RenderCore::ShaderHandle LoadShaderFromFile(RenderCore::RenderDevice* Device, const char* Filename, RenderCore::ShaderStage Stage) {
        IO::FileSystem FileSystem;

        IO::FileHandle* Handle = FileSystem.OpenFileRead(Filename);
        if (!Handle) return RenderCore::ShaderHandle{};

        usize FileSize = Handle->GetSize();
        char* Buffer = new char[FileSize + 1]{};

        Handle->Read(Buffer, FileSize);

        RenderCore::ShaderDesc ShaderDesc = {};
        ShaderDesc.Stage = Stage;
        ShaderDesc.Source = Buffer;
        ShaderDesc.DebugName = Filename;

        RenderCore::ShaderHandle Shader = Device->CreateShader(ShaderDesc);

        FileSystem.CloseFile(Handle);

        return Shader;
    }

    inline static RenderCore::ProgramHandle CreateShaderProgram(RenderCore::RenderDevice* Device, const char* VertexShaderPath, const char* FragmentShaderPath, const char* DebugName) {
        RenderCore::ShaderHandle VertexShader = LoadShaderFromFile(Device, VertexShaderPath, RenderCore::ShaderStage::Vertex);
        if (!VertexShader) {
            return RenderCore::ProgramHandle{};
        }

        RenderCore::ShaderHandle FragmentShader = LoadShaderFromFile(Device, FragmentShaderPath, RenderCore::ShaderStage::Fragment);
        if (!FragmentShader) {
            Device->DestroyShader(VertexShader);
            return RenderCore::ProgramHandle{};
        }

        RenderCore::ProgramDesc ProgramDesc = {};
        ProgramDesc.VertexShader = VertexShader;
        ProgramDesc.FragmentShader = FragmentShader;
        ProgramDesc.DebugName = DebugName;

        RenderCore::ProgramHandle Program = Device->CreateProgram(ProgramDesc);

        Device->DestroyShader(VertexShader);
        Device->DestroyShader(FragmentShader);

        return Program;
    }

    static inline RenderCore::PipelineHandle CreatePipeline(RenderCore::RenderDevice* Device, RenderCore::ProgramHandle Program, MaterialType Type) {
        RenderCore::VertexLayoutDesc VertexLayoutDesc;
        // Configure pipeline based on material type
        switch (Type) {
            case MaterialType::Opaque:
            case MaterialType::Transparent: {
                VertexLayoutDesc.AttributeCount = 3;
                VertexLayoutDesc.Attributes[0] = { 0, 0, RenderCore::VertexAttribFormat::Float3, 0 };   // Position
                VertexLayoutDesc.Attributes[1] = { 1, 0, RenderCore::VertexAttribFormat::Float3, 12 };  // Normal
                VertexLayoutDesc.Attributes[2] = { 2, 0, RenderCore::VertexAttribFormat::Float2, 24 };  // TexCoord
                VertexLayoutDesc.BindingCount = 1;
                VertexLayoutDesc.Bindings[0] = { 32, 0 }; // Stride, Divisor
            } break;
            case MaterialType::Unlit: {
                VertexLayoutDesc.AttributeCount = 2;
                VertexLayoutDesc.Attributes[0] = { 0, 0, RenderCore::VertexAttribFormat::Float3, 0 };   // Position
                VertexLayoutDesc.Attributes[1] = { 1, 0, RenderCore::VertexAttribFormat::Float2, 12 };  // TexCoord
                VertexLayoutDesc.BindingCount = 1;
                VertexLayoutDesc.Bindings[0] = { 20, 0 }; // Stride, Divisor
            } break;
        }

        RenderCore::PipelineDesc PipelineDesc;
        PipelineDesc.Program = Program;
        PipelineDesc.VertexLayout = Device->CreateVertexLayout(VertexLayoutDesc);

        RenderCore::PipelineHandle Pipeline = Device->CreatePipeline(PipelineDesc);
        return Pipeline;
    }

    RenderSystem::RenderSystem(RenderCore::RenderDevice* InDevice) {
        Impl = new RenderSystemImpl();
        Impl->Device = InDevice;

        Impl->OpaqueShaderProgram = CreateShaderProgram(Impl->Device, "Shaders/Opaque.vert", "Shaders/Opaque.frag", "OpaqueShaderProgram");
        Impl->TransparentShaderProgram = CreateShaderProgram(Impl->Device, "Shaders/Transparent.vert", "Shaders/Transparent.frag", "TransparentShaderProgram");
        Impl->UnlitShaderProgram = CreateShaderProgram(Impl->Device, "Shaders/Unlit.vert", "Shaders/Unlit.frag", "UnlitShaderProgram");
        
        Impl->OpaquePipeline = CreatePipeline(Impl->Device, Impl->OpaqueShaderProgram, MaterialType::Opaque);
        Impl->TransparentPipeline = CreatePipeline(Impl->Device, Impl->TransparentShaderProgram, MaterialType::Transparent);
        Impl->UnlitPipeline = CreatePipeline(Impl->Device, Impl->UnlitShaderProgram, MaterialType::Unlit);
    }

    RenderSystem::~RenderSystem() {

    }

    void RenderSystem::BeginFrame(const Math::Matrix4& ViewMatrix, const Math::Matrix4& ProjectionMatrix) {
        Impl->CurrentViewMatrix = ViewMatrix;
        Impl->CurrentProjectionMatrix = ProjectionMatrix;
    }

    void RenderSystem::EndFrame() {
        FlushDrawCommands();
        Impl->DrawCommandCount = 0;
    }

    MeshHandle RenderSystem::CreateMesh(const Vertex* Vertices, usize VertexCount, const u32* Indices, usize IndexCount) {
        return Impl->MeshTable.Create([&](MeshRecord& Mesh) {
            RenderCore::BufferDesc VertexBufferDesc;
            VertexBufferDesc.Type = RenderCore::BufferType::Vertex;
            VertexBufferDesc.Usage = RenderCore::BufferUsage::Static;
            VertexBufferDesc.SizeInBytes = VertexCount * sizeof(Vertex);
            VertexBufferDesc.InitialData = Vertices;

            RenderCore::BufferDesc IndexBufferDesc;
            IndexBufferDesc.Type = RenderCore::BufferType::Index;
            IndexBufferDesc.Usage = RenderCore::BufferUsage::Static;
            IndexBufferDesc.SizeInBytes = IndexCount * sizeof(u32);
            IndexBufferDesc.InitialData = Indices;

            Mesh.Handle = Mesh.Handle;
            Mesh.VertexBuffer = Impl->Device->CreateBuffer(VertexBufferDesc);
            Mesh.IndexBuffer = Impl->Device->CreateBuffer(IndexBufferDesc);
            Mesh.IndexCount = IndexCount;
        });
    }

    void RenderSystem::DestroyMesh(MeshHandle Mesh) {
        Impl->MeshTable.Destroy(Mesh, [&](MeshRecord& MeshRecord) {
            Impl->Device->DestroyBuffer(MeshRecord.VertexBuffer);
            Impl->Device->DestroyBuffer(MeshRecord.IndexBuffer);
        });
    }

    MaterialHandle RenderSystem::CreateMaterial(MaterialType Type) {
        return Impl->MaterialTable.Create([&](MaterialRecord& Material) {
            Material.Handle = Material.Handle;

            switch (Type) {
                case MaterialType::Opaque:
                    Material.Program = Impl->OpaqueShaderProgram;
                    Material.Pipeline = Impl->OpaquePipeline;
                    Material.DiffuseColor = Math::Vector4(1.0f, 1.0f, 1.0f, 1.0f);
                    break;
                case MaterialType::Transparent:
                    Material.Program = Impl->TransparentShaderProgram;
                    Material.Pipeline = Impl->TransparentPipeline;
                    Material.DiffuseColor = Math::Vector4(1.0f, 1.0f, 1.0f, 0.5f);
                    break;
                case MaterialType::Unlit:
                    Material.Program = Impl->UnlitShaderProgram;
                    Material.Pipeline = Impl->UnlitPipeline;
                    Material.DiffuseColor = Math::Vector4(1.0f, 1.0f, 1.0f, 1.0f);
                    break;
            }

        });
    }

    void RenderSystem::DestroyMaterial(MaterialHandle Material) {
        Impl->MaterialTable.Destroy(Material, [&](MaterialRecord& MaterialRecord) {
            // Note: Shader programs and pipelines are shared; do not destroy here
        });
    }
    
    void RenderSystem::Submit(MeshHandle Mesh, MaterialHandle Material, const Math::Matrix4& Transform) {
        if (Impl->DrawCommandCount >= MaxDrawCommands) {
            FlushDrawCommands();
        }

        DrawCommand& Cmd = Impl->DrawCommands[Impl->DrawCommandCount++];
        Cmd.Material = Material;
        Cmd.Pipeline = Impl->MaterialTable.TryGet(Material)->Pipeline;
        Cmd.VertexBuffer = Impl->MeshTable.TryGet(Mesh)->VertexBuffer;
        Cmd.IndexBuffer = Impl->MeshTable.TryGet(Mesh)->IndexBuffer;
        Cmd.Transform = Transform;
        Cmd.IndexCount = static_cast<u32>(Impl->MeshTable.TryGet(Mesh)->IndexCount);
        Cmd.IndexOffset = 0;
        Cmd.VertexOffset = 0;
    }

    void RenderSystem::FlushDrawCommands() {
        RenderCore::RenderDevice* Device = Impl->Device;

        RenderCore::RenderPassDesc OpaquePassDesc = {};
        OpaquePassDesc.ClearColor = true;
        OpaquePassDesc.ClearDepth = true;
        OpaquePassDesc.ClearColorValue = Math::Vector4(0.1f, 0.1f, 0.1f, 1.0f);
        OpaquePassDesc.ClearDepthValue = 1.0f;

        Device->BeginRenderPass(OpaquePassDesc);

        for (usize i = 0; i < Impl->DrawCommandCount; ++i) {
            const DrawCommand& Cmd = Impl->DrawCommands[i];

            const MaterialRecord* Material = Impl->MaterialTable.TryGet(Cmd.Material);
            if (!Material) continue;

            Device->BindPipeline(Cmd.Pipeline);
            Device->SetUniformMat4(Material->Program, "uModelMatrix", Cmd.Transform.m);
            Device->SetUniformMat4(Material->Program, "uViewMatrix", Impl->CurrentViewMatrix.m);
            Device->SetUniformMat4(Material->Program, "uProjectionMatrix", Impl->CurrentProjectionMatrix.m);
            Device->SetUniformVec4(Material->Program, "uDiffuseColor", &Material->DiffuseColor.x);

            Device->BindVertexBuffer(Cmd.VertexBuffer, 0, 0, sizeof(Vertex));
            Device->BindBuffer(Cmd.IndexBuffer);

            Device->DrawIndexed(Cmd.IndexCount, Cmd.IndexOffset, Cmd.VertexOffset);
        }

        Impl->DrawCommandCount = 0;
        Device->EndRenderPass();
    }
    
}