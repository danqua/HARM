#include "Engine/RenderCore/RenderDeviceGL.h"
#include <glad/glad.h>

#include <cassert>
#include <cstdio>

namespace Engine::RenderCore {

    static inline GLenum GetBufferType(BufferType Type) {
        switch (Type) {
            case BufferType::Vertex: return GL_ARRAY_BUFFER;
            case BufferType::Index: return GL_ELEMENT_ARRAY_BUFFER;
            case BufferType::Uniform: return GL_UNIFORM_BUFFER;
            default: return 0;
        }
    }

    static inline GLenum GetBufferUsage(BufferUsage Usage) {
        switch (Usage) {
            case BufferUsage::Static: return GL_STATIC_DRAW;
            case BufferUsage::Dynamic: return GL_DYNAMIC_DRAW;
            case BufferUsage::Stream: return GL_STREAM_DRAW;
            default: return 0;
        }
    }

    static inline GLenum GetShaderType(ShaderStage glTexStorage3DMultisample) {
        switch (glTexStorage3DMultisample) {
            case ShaderStage::Vertex: return GL_VERTEX_SHADER;
            case ShaderStage::Fragment: return GL_FRAGMENT_SHADER;
            case ShaderStage::Compute: return GL_COMPUTE_SHADER;
            default: return 0;
        }
    }

    static inline GLenum GetTextureFormat(TextureFormat Format) {
        switch (Format) {
            case TextureFormat::RGBA8: return GL_RGBA8;
            case TextureFormat::RGB8: return GL_RGB8;
            case TextureFormat::D24S8: return GL_DEPTH24_STENCIL8;
            default: return 0;
        }
    }

    static inline GLenum GetAttribType(VertexAttribFormat Format) {
        switch (Format) {
            case VertexAttribFormat::Float: return GL_FLOAT;
            case VertexAttribFormat::Float2: return GL_FLOAT;
            case VertexAttribFormat::Float3: return GL_FLOAT;
            case VertexAttribFormat::Float4: return GL_FLOAT;
            default: return 0;
        }
    }

    static inline u32 GetAttribComponentCount(VertexAttribFormat Format) {
        switch (Format) {
            case VertexAttribFormat::Float: return 1;
            case VertexAttribFormat::Float2: return 2;
            case VertexAttribFormat::Float3: return 3;
            case VertexAttribFormat::Float4: return 4;
            default: return 0;
        }
    }

    static inline GLenum GetCullMode(CullMode Cull) {
        switch (Cull) {
            case CullMode::None: return GL_NONE;
            case CullMode::Front: return GL_FRONT;
            case CullMode::Back: return GL_BACK;
            default: return GL_NONE;
        }
    }

    static inline GLenum GetFrontFace(FrontFace Face) {
        switch (Face) {
            case FrontFace::Clockwise: return GL_CW;
            case FrontFace::CounterClockwise: return GL_CCW;
            default: return GL_CCW;
        }
    }

    static inline GLenum GetCompareOp(CompareOp Op) {
        switch (Op) {
            case CompareOp::Never: return GL_NEVER;
            case CompareOp::Less: return GL_LESS;
            case CompareOp::Equal: return GL_EQUAL;
            case CompareOp::LessEqual: return GL_LEQUAL;
            case CompareOp::Greater: return GL_GREATER;
            case CompareOp::NotEqual: return GL_NOTEQUAL;
            default: return GL_LESS;
        }
    }

    static inline GLenum GetTopology(PrimitiveTopology Topology) {
        switch (Topology) {
            case PrimitiveTopology::Triangles: return GL_TRIANGLES;
            case PrimitiveTopology::Lines: return GL_LINES;
            case PrimitiveTopology::Points: return GL_POINTS;
            default: return GL_TRIANGLES;
        }
    }

    static void APIENTRY GLDebugMessageCallback(GLenum Source, GLenum Type, GLuint Id,
                                                GLenum Severity, GLsizei Length,
                                            const GLchar* Message, const void* UserParam) {
        // TODO: Replace with engine logging system                                        
        printf("OpenGL Debug Message: %s\n", Message);
    }

    RenderDevice::RenderDevice(const RenderDeviceDesc& Desc) {

        assert(gladLoadGL() && "Failed to initialize GLAD\n");

        GLint Flags = 0;
        glGetIntegerv(GL_CONTEXT_FLAGS, &Flags);
        if (Flags & GL_CONTEXT_FLAG_DEBUG_BIT) {
            glEnable(GL_DEBUG_OUTPUT);
            glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
            glDebugMessageCallback(GLDebugMessageCallback, nullptr);
            glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, GL_FALSE);
        }

        Impl = new RenderDeviceImpl();
    }

    RenderDevice::~RenderDevice() {
        delete Impl;
    }

    BufferHandle RenderDevice::CreateBuffer(const BufferDesc& Desc) {
        return Impl->Buffers.Create([&](GLBuffer& Buffer) {
            GLuint BufferId;
            glGenBuffers(1, &BufferId);

            GLenum Target = GetBufferType(Desc.Type);
            GLenum Usage = GetBufferUsage(Desc.Usage);

            glBindBuffer(Target, BufferId);
            glBufferData(Target, Desc.SizeInBytes, Desc.InitialData, Usage);

            Buffer.Id = BufferId;
            Buffer.Type = Desc.Type;
            Buffer.Usage = Desc.Usage;
            Buffer.SizeInBytes = Desc.SizeInBytes;
        });
    }

    void RenderDevice::DestroyBuffer(BufferHandle Buffer) {
        Impl->Buffers.Destroy(Buffer, [&](GLBuffer& B) {
            if (B.Id != 0) {
                glDeleteBuffers(1, &B.Id);
                B.Id = 0;
            }
        });
    }

    void RenderDevice::BindBuffer(BufferHandle Buffer) {
        GLBuffer* B = Impl->Buffers.TryGet(Buffer);
        if (!B) return;
    
        GLenum Target = GetBufferType(B->Type);
        glBindBuffer(Target, B->Id);
    }

    void RenderDevice::BindVertexBuffer(BufferHandle Buffer, u32 BindingIndex, usize Offset, usize Stride) {
        GLBuffer* B = Impl->Buffers.TryGet(Buffer);
        if (!B) return;

        glBindVertexBuffer(BindingIndex, B->Id, static_cast<GLintptr>(Offset), static_cast<GLsizei>(Stride));
    }

    void RenderDevice::UpdateBuffer(BufferHandle Buffer, const void* Data, usize SizeInBytes, usize Offset) {
        GLBuffer* B = Impl->Buffers.TryGet(Buffer);
        if (!B) return;
    
        GLenum Target = GetBufferType(B->Type);
        glBindBuffer(Target, B->Id);
        glBufferSubData(Target, static_cast<GLintptr>(Offset), static_cast<GLsizeiptr>(SizeInBytes), Data);
    }
    
    ShaderHandle RenderDevice::CreateShader(const ShaderDesc& Desc) {
        return Impl->Shaders.Create([&](GLShader& Shader) {
            GLenum GlShaderType = GetShaderType(Desc.Stage);
            GLuint ShaderId = glCreateShader(GlShaderType);

            glShaderSource(ShaderId, 1, &Desc.Source, nullptr);
            glCompileShader(ShaderId);

            GLint Success;
            glGetShaderiv(ShaderId, GL_COMPILE_STATUS, &Success);

            if (!Success) {
                char InfoLog[512];
                glGetShaderInfoLog(ShaderId, 512, nullptr, InfoLog);
                // TODO: Replace with engine logging system
                printf("Shader compilation failed: %s\n", InfoLog);
                glDeleteShader(ShaderId);
                Shader.Id = 0;
                return;
            }

            Shader.Id = ShaderId;
        });
    }

    void RenderDevice::DestroyShader(ShaderHandle Shader) {
        Impl->Shaders.Destroy(Shader, [&](GLShader& S) {
            if (S.Id != 0) {
                glDeleteShader(S.Id);
                S.Id = 0;
            }
        });
    }

    ProgramHandle RenderDevice::CreateProgram(const ProgramDesc& Desc) {
        return Impl->Programs.Create([&](GLProgram& Program) {
            GLShader* VertexShader = Impl->Shaders.TryGet(Desc.VertexShader);
            GLShader* FragmentShader = Impl->Shaders.TryGet(Desc.FragmentShader);

            if (!VertexShader || !FragmentShader) {
                Program.Id = 0;
                return;
            }

            GLuint GlProgram = glCreateProgram();
            glAttachShader(GlProgram, VertexShader->Id);
            glAttachShader(GlProgram, FragmentShader->Id);
            glLinkProgram(GlProgram);

            GLint Success;
            glGetProgramiv(GlProgram, GL_LINK_STATUS, &Success);
            if (!Success) {
                char InfoLog[512];
                glGetProgramInfoLog(GlProgram, 512, nullptr, InfoLog);
                // TODO: Replace with engine logging system
                printf("Program linking failed: %s\n", InfoLog);
                glDeleteProgram(GlProgram);
                Program.Id = 0;
                return;
            }
            Program.Id = GlProgram;
        });
    }

    void RenderDevice::DestroyProgram(ProgramHandle Program) {
        Impl->Programs.Destroy(Program, [&](GLProgram& P) {
            if (P.Id != 0) {
                glDeleteProgram(P.Id);
                P.Id = 0;
            }
        });
    }

    void RenderDevice::BindProgram(ProgramHandle Program) {
        GLProgram* P = Impl->Programs.TryGet(Program);
        if (P && Impl->CurrentProgram != P->Id) {
            glUseProgram(P->Id);
            Impl->CurrentProgram = P->Id;
        }
    }

    void RenderDevice::SetUniformInt(ProgramHandle Program, const char* Name, int Value) {
        GLProgram* P = Impl->Programs.TryGet(Program);
        
        if (!P) return;

        GLint Location = glGetUniformLocation(P->Id, Name);
        if (Location != -1) {
            glUniform1i(Location, Value);
        }
    }

    void RenderDevice::SetUniformFloat(ProgramHandle Program, const char* Name, float Value) {
        GLProgram* P = Impl->Programs.TryGet(Program);
        if (!P) return;

        GLint Location = glGetUniformLocation(P->Id, Name);
        if (Location != -1) {
            glUniform1f(Location, Value);
        }
    }

    void RenderDevice::SetUniformVec2(ProgramHandle Program, const char* Name, const float* Value) {
        GLProgram* P = Impl->Programs.TryGet(Program);
        if (!P) return;

        GLint Location = glGetUniformLocation(P->Id, Name);
        if (Location != -1) {
            glUniform2fv(Location, 1, Value);
        }
    }

    void RenderDevice::SetUniformVec3(ProgramHandle Program, const char* Name, const float* Value) {
        GLProgram* P = Impl->Programs.TryGet(Program);
        if (!P) return;

        GLint Location = glGetUniformLocation(P->Id, Name);
        if (Location != -1) {
            glUniform3fv(Location, 1, Value);
        }
    }

    void RenderDevice::SetUniformVec4(ProgramHandle Program, const char* Name, const float* Value) {
        GLProgram* P = Impl->Programs.TryGet(Program);
        if (!P) return;

        GLint Location = glGetUniformLocation(P->Id, Name);
        if (Location != -1) {
            glUniform4fv(Location, 1, Value);
        }
    }

    void RenderDevice::SetUniformMat4(ProgramHandle Program, const char* Name, const float* Value) {
        GLProgram* P = Impl->Programs.TryGet(Program);
        if (!P) return;

        GLint Location = glGetUniformLocation(P->Id, Name);
        if (Location != -1) {
            glUniformMatrix4fv(Location, 1, GL_FALSE, Value);
        }
    }

    TextureHandle RenderDevice::CreateTexture(const TextureDesc& Desc) {
        return Impl->Textures.Create([&](GLTexture& Texture) {
            GLuint TextureId;
            glGenTextures(1, &TextureId);

            GLenum Format = GetTextureFormat(Desc.Format);

            glBindTexture(GL_TEXTURE_2D, TextureId);
            glTexImage2D(GL_TEXTURE_2D, 0, Format, Desc.Width, Desc.Height, 0,
                         (Format == GL_DEPTH24_STENCIL8) ? GL_DEPTH_STENCIL : GL_RGBA,
                         (Format == GL_DEPTH24_STENCIL8) ? GL_UNSIGNED_INT_24_8 : GL_UNSIGNED_BYTE,
                         Desc.InitialData);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

            Texture.Id = TextureId;
            Texture.Width = Desc.Width;
            Texture.Height = Desc.Height;
            Texture.Format = Desc.Format;
        });
    }

    void RenderDevice::DestroyTexture(TextureHandle Texture) {
        Impl->Textures.Destroy(Texture, [&](GLTexture& T) {
            if (T.Id != 0) {
                glDeleteTextures(1, &T.Id);
                T.Id = 0;
            }
        });
    }

    void RenderDevice::BindTexture(TextureHandle Texture, u32 Slot) {
        GLTexture* T = Impl->Textures.TryGet(Texture);
        if (!T) return;

        glActiveTexture(GL_TEXTURE0 + Slot);
        glBindTexture(GL_TEXTURE_2D, T->Id);
    }

    void RenderDevice::UpdateTexture(TextureHandle Texture, const void* Data) {
        GLTexture* T = Impl->Textures.TryGet(Texture);
        if (!T) return;

        GLenum Format = GetTextureFormat(T->Format);
        glBindTexture(GL_TEXTURE_2D, T->Id);
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, T->Width, T->Height,
                            (Format == GL_DEPTH24_STENCIL8) ? GL_DEPTH_STENCIL : GL_RGBA,
                            (Format == GL_DEPTH24_STENCIL8) ? GL_UNSIGNED_INT_24_8 : GL_UNSIGNED_BYTE,
                            Data);
    }

    VertexLayoutHandle RenderDevice::CreateVertexLayout(const VertexLayoutDesc& Desc) {
        return Impl->VertexLayouts.Create([&](VertexLayout& Layout) {
            GLuint Vao;
            glGenVertexArrays(1, &Vao);
            glBindVertexArray(Vao);

            for (u32 i = 0; i < Desc.AttributeCount; ++i) {
                const VertexAttributeDesc& Attr = Desc.Attributes[i];
                GLenum AttribType = GetAttribType(Attr.Format);
                u32 ComponentCount = GetAttribComponentCount(Attr.Format);

                glEnableVertexAttribArray(Attr.Location);
                glVertexAttribFormat(Attr.Location, ComponentCount, AttribType, GL_FALSE, Attr.Offset);
                glVertexAttribBinding(Attr.Location, Attr.Binding);
            }

            for (u32 i = 0; i < Desc.BindingCount; ++i) {
                const VertexBinding& Binding = Desc.Bindings[i];
                glVertexBindingDivisor(i, Binding.Divisor);
            }

            glBindVertexArray(0);

            Layout.Vao = Vao;
        });
    }

    void RenderDevice::DestroyVertexLayout(VertexLayoutHandle Layout) {
        Impl->VertexLayouts.Destroy(Layout, [&](VertexLayout& L) {
            if (L.Vao != 0) {
                glDeleteVertexArrays(1, &L.Vao);
                L.Vao = 0;
            }
        });
    }

    PipelineHandle RenderDevice::CreatePipeline(const PipelineDesc& Desc) {
        return Impl->Pipelines.Create([&](GLPipeline& Pipeline) {
            Pipeline.Program = Desc.Program;
            Pipeline.Topology = Desc.Topology;
            Pipeline.VertexLayout = Desc.VertexLayout;
            Pipeline.DepthTest = Desc.DepthTest;
            Pipeline.DepthWrite = Desc.DepthWrite;
            Pipeline.DepthCompare = Desc.DepthCompare;
            Pipeline.Front = Desc.Front;
            Pipeline.Cull = Desc.Cull;
            Pipeline.Wireframe = Desc.Wireframe;
        });
    }

    void RenderDevice::DestroyPipeline(PipelineHandle Pipeline) {
        Impl->Pipelines.Destroy(Pipeline, [&](GLPipeline& P) {
            // No GPU resources to free for pipeline in this implementation
        });
    }

    void RenderDevice::BindPipeline(PipelineHandle Pipeline) {
        GLPipeline* P = Impl->Pipelines.TryGet(Pipeline);
        if (!P) return;

        // Bind program
        BindProgram(P->Program);

        // Bind vertex layout
        VertexLayout* Layout = Impl->VertexLayouts.TryGet(P->VertexLayout);
        if (Layout) {
            glBindVertexArray(Layout->Vao);
        }

        // Set depth test
        if (P->DepthTest) {
            glEnable(GL_DEPTH_TEST);
            GLenum DepthFunc = GetCompareOp(P->DepthCompare);
            glDepthFunc(DepthFunc);
        } else {
            glDisable(GL_DEPTH_TEST);
        }

        // Set depth write
        glDepthMask(P->DepthWrite ? GL_TRUE : GL_FALSE);

        // Set cull mode
        if (P->Cull != CullMode::None) {
            glEnable(GL_CULL_FACE);
            glCullFace(GetCullMode(P->Cull));
        } else {
            glDisable(GL_CULL_FACE);
        }

        // Set front face
        glFrontFace(GetFrontFace(P->Front));

        // Set polygon mode
        if (P->Wireframe) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        } else {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }

        // Update current topology
        Impl->CurrentTopology = P->Topology;
    }

    FramebufferHandle RenderDevice::CreateFramebuffer(const FramebufferDesc& Desc) {
        return Impl->Framebuffers.Create([&](GLFramebuffer& Framebuffer) {
            GLuint Fbo;
            glGenFramebuffers(1, &Fbo);
            glBindFramebuffer(GL_FRAMEBUFFER, Fbo);

            Framebuffer.ColorAttachmentCount = 0;

            for (u32 i = 0; i < 4; ++i) {
                TextureHandle ColorTexHandle = Desc.ColorAttachments[i];
                if (ColorTexHandle) {
                    GLTexture* ColorTex = Impl->Textures.TryGet(ColorTexHandle);
                    if (ColorTex) {
                        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i,
                                               GL_TEXTURE_2D, ColorTex->Id, 0);
                        Framebuffer.ColorAttachments[Framebuffer.ColorAttachmentCount++] = ColorTexHandle;
                    }
                }
            }

            TextureHandle DepthTexHandle = Desc.DepthAttachment;
            if (DepthTexHandle) {
                GLTexture* DepthTex = Impl->Textures.TryGet(DepthTexHandle);
                if (DepthTex) {
                    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
                                           GL_TEXTURE_2D, DepthTex->Id, 0);
                    Framebuffer.DepthAttachment = DepthTexHandle;
                }
            }

            GLenum Status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
            if (Status != GL_FRAMEBUFFER_COMPLETE) {
                // TODO: Replace with engine logging system
                printf("Framebuffer is not complete: 0x%X\n", Status);
                glDeleteFramebuffers(1, &Fbo);
                Framebuffer.Id = 0;
                return;
            }

            Framebuffer.Id = Fbo;
            Framebuffer.Width = Desc.Width;
            Framebuffer.Height = Desc.Height;
        });
    }

    void RenderDevice::DestroyFramebuffer(FramebufferHandle Framebuffer) {
        Impl->Framebuffers.Destroy(Framebuffer, [&](GLFramebuffer& F) {
            if (F.Id != 0) {
                glDeleteFramebuffers(1, &F.Id);
                F.Id = 0;
            }
        });
    }

    void RenderDevice::BeginRenderPass(const RenderPassDesc& Desc) {
        GLFramebuffer* Fbo = Impl->Framebuffers.TryGet(Desc.Framebuffer);
        if (Fbo) {
            glBindFramebuffer(GL_FRAMEBUFFER, Fbo->Id);
            glViewport(0, 0, Fbo->Width, Fbo->Height);
        } else {
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glViewport(0, 0, Impl->DefaultFramebufferWidth, Impl->DefaultFramebufferHeight);
        }

        GLbitfield ClearFlags = 0;
        if (Desc.ClearColor) {
            glClearColor(Desc.ClearColorValue.x, Desc.ClearColorValue.y,
                            Desc.ClearColorValue.z, Desc.ClearColorValue.w);
            ClearFlags |= GL_COLOR_BUFFER_BIT;
        }
        if (Desc.ClearDepth) {
            glClearDepth(Desc.ClearDepthValue);
            ClearFlags |= GL_DEPTH_BUFFER_BIT;
        }
        if (ClearFlags != 0) {
            glClear(ClearFlags);
        }
    }

    void RenderDevice::EndRenderPass() {
        // Do nothing for now
    }

    void RenderDevice::Draw(u32 VertexCount, u32 StartVertex) {
        glDrawArrays(GetTopology(Impl->CurrentTopology), static_cast<GLint>(StartVertex), static_cast<GLsizei>(VertexCount));
    }

    void RenderDevice::DrawIndexed(u32 IndexCount, u32 StartIndex, s32 BaseVertex) {
        glDrawElementsBaseVertex(GetTopology(Impl->CurrentTopology),
                                 static_cast<GLsizei>(IndexCount),
                                 GL_UNSIGNED_INT,
                                 reinterpret_cast<const void*>(static_cast<uintptr_t>(StartIndex * sizeof(u32))),
                                 static_cast<GLint>(BaseVertex));
    }

}