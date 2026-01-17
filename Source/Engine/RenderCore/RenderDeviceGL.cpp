#include "Engine/RenderCore/RenderDeviceGL.h"
#include <glad/glad.h>

#include <cassert>
#include <cstdio>

namespace Hx {

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

    static inline GLenum GetShaderType(ShaderStage stage) {
        switch (stage) {
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

    RenderDevice::RenderDevice(const RenderDeviceDesc& desc) {

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

    BufferHandle RenderDevice::CreateBuffer(const BufferDesc& desc) {
        return Impl->buffers.Create([&](GLBuffer& buffer) {
            GLuint BufferId;
            glGenBuffers(1, &BufferId);

            GLenum Target = GetBufferType(desc.type);
            GLenum Usage = GetBufferUsage(desc.usage);

            glBindBuffer(Target, BufferId);
            glBufferData(Target, desc.sizeInBytes, desc.initialData, Usage);

            buffer.id = BufferId;
            buffer.type = desc.type;
            buffer.usage = desc.usage;
            buffer.sizeInBytes = desc.sizeInBytes;
        });
    }

    void RenderDevice::DestroyBuffer(BufferHandle Buffer) {
        Impl->buffers.Destroy(Buffer, [&](GLBuffer& B) {
            if (B.id != 0) {
                glDeleteBuffers(1, &B.id);
                B.id = 0;
            }
        });
    }

    void RenderDevice::BindBuffer(BufferHandle Buffer) {
        GLBuffer* B = Impl->buffers.TryGet(Buffer);
        if (!B) return;
    
        GLenum Target = GetBufferType(B->type);
        glBindBuffer(Target, B->id);
    }

    void RenderDevice::BindVertexBuffer(BufferHandle Buffer, u32 BindingIndex, usize Offset, usize Stride) {
        GLBuffer* B = Impl->buffers.TryGet(Buffer);
        if (!B) return;

        glBindVertexBuffer(BindingIndex, B->id, static_cast<GLintptr>(Offset), static_cast<GLsizei>(Stride));
    }

    void RenderDevice::UpdateBuffer(BufferHandle Buffer, const void* Data, usize SizeInBytes, usize Offset) {
        GLBuffer* B = Impl->buffers.TryGet(Buffer);
        if (!B) return;
    
        GLenum Target = GetBufferType(B->type);
        glBindBuffer(Target, B->id);
        glBufferSubData(Target, static_cast<GLintptr>(Offset), static_cast<GLsizeiptr>(SizeInBytes), Data);
    }
    
    ShaderHandle RenderDevice::CreateShader(const ShaderDesc& desc) {
        return Impl->shaders.Create([&](GLShader& shader) {
            GLenum GlShaderType = GetShaderType(desc.stage);
            GLuint ShaderId = glCreateShader(GlShaderType);

            glShaderSource(ShaderId, 1, &desc.source, nullptr);
            glCompileShader(ShaderId);

            GLint Success;
            glGetShaderiv(ShaderId, GL_COMPILE_STATUS, &Success);

            if (!Success) {
                char InfoLog[512];
                glGetShaderInfoLog(ShaderId, 512, nullptr, InfoLog);
                // TODO: Replace with engine logging system
                printf("Shader compilation failed: %s\n", InfoLog);
                glDeleteShader(ShaderId);
                shader.id = 0;
                return;
            }

            shader.id = ShaderId;
        });
    }

    void RenderDevice::DestroyShader(ShaderHandle Shader) {
        Impl->shaders.Destroy(Shader, [&](GLShader& S) {
            if (S.id != 0) {
                glDeleteShader(S.id);
                S.id = 0;
            }
        });
    }

    ProgramHandle RenderDevice::CreateProgram(const ProgramDesc& desc) {
        return Impl->programs.Create([&](GLProgram& program) {
            GLShader* VertexShader = Impl->shaders.TryGet(desc.vertexShader);
            GLShader* FragmentShader = Impl->shaders.TryGet(desc.fragmentShader);

            if (!VertexShader || !FragmentShader) {
                program.id = 0;
                return;
            }

            GLuint GlProgram = glCreateProgram();
            glAttachShader(GlProgram, VertexShader->id);
            glAttachShader(GlProgram, FragmentShader->id);
            glLinkProgram(GlProgram);

            GLint Success;
            glGetProgramiv(GlProgram, GL_LINK_STATUS, &Success);
            if (!Success) {
                char InfoLog[512];
                glGetProgramInfoLog(GlProgram, 512, nullptr, InfoLog);
                // TODO: Replace with engine logging system
                printf("Program linking failed: %s\n", InfoLog);
                glDeleteProgram(GlProgram);
                program.id = 0;
                return;
            }
            program.id = GlProgram;
        });
    }

    void RenderDevice::DestroyProgram(ProgramHandle Program) {
        Impl->programs.Destroy(Program, [&](GLProgram& P) {
            if (P.id != 0) {
                glDeleteProgram(P.id);
                P.id = 0;
            }
        });
    }

    void RenderDevice::BindProgram(ProgramHandle Program) {
        GLProgram* P = Impl->programs.TryGet(Program);
        if (P && Impl->currentProgram != P->id) {
            glUseProgram(P->id);
            Impl->currentProgram = P->id;
        }
    }

    void RenderDevice::SetUniformInt(ProgramHandle Program, const char* Name, int Value) {
        GLProgram* P = Impl->programs.TryGet(Program);
        
        if (!P) return;

        GLint Location = glGetUniformLocation(P->id, Name);
        if (Location != -1) {
            glUniform1i(Location, Value);
        }
    }

    void RenderDevice::SetUniformFloat(ProgramHandle Program, const char* Name, float Value) {
        GLProgram* P = Impl->programs.TryGet(Program);
        if (!P) return;

        GLint Location = glGetUniformLocation(P->id, Name);
        if (Location != -1) {
            glUniform1f(Location, Value);
        }
    }

    void RenderDevice::SetUniformVec2(ProgramHandle Program, const char* Name, const float* Value) {
        GLProgram* P = Impl->programs.TryGet(Program);
        if (!P) return;

        GLint Location = glGetUniformLocation(P->id, Name);
        if (Location != -1) {
            glUniform2fv(Location, 1, Value);
        }
    }

    void RenderDevice::SetUniformVec3(ProgramHandle Program, const char* Name, const float* Value) {
        GLProgram* P = Impl->programs.TryGet(Program);
        if (!P) return;

        GLint Location = glGetUniformLocation(P->id, Name);
        if (Location != -1) {
            glUniform3fv(Location, 1, Value);
        }
    }

    void RenderDevice::SetUniformVec4(ProgramHandle Program, const char* Name, const float* Value) {
        GLProgram* P = Impl->programs.TryGet(Program);
        if (!P) return;

        GLint Location = glGetUniformLocation(P->id, Name);
        if (Location != -1) {
            glUniform4fv(Location, 1, Value);
        }
    }

    void RenderDevice::SetUniformMat4(ProgramHandle Program, const char* Name, const float* Value) {
        GLProgram* P = Impl->programs.TryGet(Program);
        if (!P) return;

        GLint Location = glGetUniformLocation(P->id, Name);
        if (Location != -1) {
            glUniformMatrix4fv(Location, 1, GL_FALSE, Value);
        }
    }

    TextureHandle RenderDevice::CreateTexture(const TextureDesc& desc) {
        return Impl->textures.Create([&](GLTexture& texture) {
            GLuint TextureId;
            glGenTextures(1, &TextureId);

            GLenum Format = GetTextureFormat(desc.format);

            glBindTexture(GL_TEXTURE_2D, TextureId);
            glTexImage2D(GL_TEXTURE_2D, 0, Format, desc.width, desc.height, 0,
                         (Format == GL_DEPTH24_STENCIL8) ? GL_DEPTH_STENCIL : GL_RGBA,
                         (Format == GL_DEPTH24_STENCIL8) ? GL_UNSIGNED_INT_24_8 : GL_UNSIGNED_BYTE,
                         desc.initialData);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

            texture.id = TextureId;
            texture.width = desc.width;
            texture.height = desc.height;
            texture.format = desc.format;
        });
    }

    void RenderDevice::DestroyTexture(TextureHandle Texture) {
        Impl->textures.Destroy(Texture, [&](GLTexture& T) {
            if (T.id != 0) {
                glDeleteTextures(1, &T.id);
                T.id = 0;
            }
        });
    }

    void RenderDevice::BindTexture(TextureHandle Texture, u32 Slot) {
        GLTexture* T = Impl->textures.TryGet(Texture);
        if (!T) return;

        glActiveTexture(GL_TEXTURE0 + Slot);
        glBindTexture(GL_TEXTURE_2D, T->id);
    }

    void RenderDevice::UpdateTexture(TextureHandle Texture, const void* Data) {
        GLTexture* T = Impl->textures.TryGet(Texture);
        if (!T) return;

        GLenum Format = GetTextureFormat(T->format);
        glBindTexture(GL_TEXTURE_2D, T->id);
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, T->width, T->height,
                            (Format == GL_DEPTH24_STENCIL8) ? GL_DEPTH_STENCIL : GL_RGBA,
                            (Format == GL_DEPTH24_STENCIL8) ? GL_UNSIGNED_INT_24_8 : GL_UNSIGNED_BYTE,
                            Data);
    }

    VertexLayoutHandle RenderDevice::CreateVertexLayout(const VertexLayoutDesc& desc) {
        return Impl->vertexLayouts.Create([&](VertexLayout& layout) {
            GLuint Vao;
            glGenVertexArrays(1, &Vao);
            glBindVertexArray(Vao);

            for (u32 i = 0; i < desc.attributeCount; ++i) {
                const VertexAttributeDesc& Attr = desc.attributes[i];
                GLenum AttribType = GetAttribType(Attr.format);
                u32 ComponentCount = GetAttribComponentCount(Attr.format);

                glEnableVertexAttribArray(Attr.location);
                glVertexAttribFormat(Attr.location, ComponentCount, AttribType, GL_FALSE, Attr.offset);
                glVertexAttribBinding(Attr.location, Attr.binding);
            }

            for (u32 i = 0; i < desc.bindingCount; ++i) {
                const VertexBinding& Binding = desc.bindings[i];
                glVertexBindingDivisor(i, Binding.divisor);
            }

            glBindVertexArray(0);

            layout.vao = Vao;
        });
    }

    void RenderDevice::DestroyVertexLayout(VertexLayoutHandle Layout) {
        Impl->vertexLayouts.Destroy(Layout, [&](VertexLayout& L) {
            if (L.vao != 0) {
                glDeleteVertexArrays(1, &L.vao);
                L.vao = 0;
            }
        });
    }

    PipelineHandle RenderDevice::CreatePipeline(const PipelineDesc& desc) {
        return Impl->pipelines.Create([&](GLPipeline& pipeline) {
            pipeline.program = desc.program;
            pipeline.topology = desc.topology;
            pipeline.vertexLayout = desc.vertexLayout;
            pipeline.depthTest = desc.depthTest;
            pipeline.depthWrite = desc.depthWrite;
            pipeline.depthCompare = desc.depthCompare;
            pipeline.front = desc.front;
            pipeline.cull = desc.cull;
            pipeline.wireframe = desc.wireframe;
        });
    }

    void RenderDevice::DestroyPipeline(PipelineHandle Pipeline) {
        Impl->pipelines.Destroy(Pipeline, [&](GLPipeline& P) {
            // No GPU resources to free for pipeline in this implementation
        });
    }

    void RenderDevice::BindPipeline(PipelineHandle Pipeline) {
        GLPipeline* P = Impl->pipelines.TryGet(Pipeline);
        if (!P) return;

        // Bind program
        BindProgram(P->program);

        // Bind vertex layout
        VertexLayout* Layout = Impl->vertexLayouts.TryGet(P->vertexLayout);
        if (Layout) {
            glBindVertexArray(Layout->vao);
        }

        // Set depth test
        if (P->depthTest) {
            glEnable(GL_DEPTH_TEST);
            GLenum DepthFunc = GetCompareOp(P->depthCompare);
            glDepthFunc(DepthFunc);
        } else {
            glDisable(GL_DEPTH_TEST);
        }

        // Set depth write
        glDepthMask(P->depthWrite ? GL_TRUE : GL_FALSE);

        // Set cull mode
        if (P->cull != CullMode::None) {
            glEnable(GL_CULL_FACE);
            glCullFace(GetCullMode(P->cull));
        } else {
            glDisable(GL_CULL_FACE);
        }

        // Set front face
        glFrontFace(GetFrontFace(P->front));

        // Set polygon mode
        if (P->wireframe) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        } else {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }

        // Update current topology
        Impl->currentTopology = P->topology;
    }

    FramebufferHandle RenderDevice::CreateFramebuffer(const FramebufferDesc& desc) {
        return Impl->framebuffers.Create([&](GLFramebuffer& framebuffer) {
            GLuint Fbo;
            glGenFramebuffers(1, &Fbo);
            glBindFramebuffer(GL_FRAMEBUFFER, Fbo);

            framebuffer.colorAttachmentCount = 0;

            for (u32 i = 0; i < 4; ++i) {
                TextureHandle ColorTexHandle = desc.colorAttachments[i];
                if (ColorTexHandle) {
                    GLTexture* ColorTex = Impl->textures.TryGet(ColorTexHandle);
                    if (ColorTex) {
                        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i,
                                               GL_TEXTURE_2D, ColorTex->id, 0);
                        framebuffer.colorAttachments[framebuffer.colorAttachmentCount++] = ColorTexHandle;
                    }
                }
            }

            TextureHandle DepthTexHandle = desc.depthAttachment;
            if (DepthTexHandle) {
                GLTexture* DepthTex = Impl->textures.TryGet(DepthTexHandle);
                if (DepthTex) {
                    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
                                           GL_TEXTURE_2D, DepthTex->id, 0);
                    framebuffer.depthAttachment = DepthTexHandle;
                }
            }

            GLenum Status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
            if (Status != GL_FRAMEBUFFER_COMPLETE) {
                // TODO: Replace with engine logging system
                printf("Framebuffer is not complete: 0x%X\n", Status);
                glDeleteFramebuffers(1, &Fbo);
                framebuffer.id = 0;
                return;
            }

            framebuffer.id = Fbo;
            framebuffer.width = desc.width;
            framebuffer.height = desc.height;
        });
    }

    void RenderDevice::DestroyFramebuffer(FramebufferHandle Framebuffer) {
        Impl->framebuffers.Destroy(Framebuffer, [&](GLFramebuffer& F) {
            if (F.id != 0) {
                glDeleteFramebuffers(1, &F.id);
                F.id = 0;
            }
        });
    }

    void RenderDevice::BeginRenderPass(const RenderPassDesc& desc) {
        GLFramebuffer* Fbo = Impl->framebuffers.TryGet(desc.framebuffer);
        if (Fbo) {
            glBindFramebuffer(GL_FRAMEBUFFER, Fbo->id);
            glViewport(0, 0, Fbo->width, Fbo->height);
        } else {
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glViewport(0, 0, Impl->defaultFramebufferWidth, Impl->defaultFramebufferHeight);
        }

        GLbitfield ClearFlags = 0;
        if (desc.clearColor) {
            glClearColor(desc.clearColorValue.x, desc.clearColorValue.y,
                            desc.clearColorValue.z, desc.clearColorValue.w);
            ClearFlags |= GL_COLOR_BUFFER_BIT;
        }
        if (desc.clearDepth) {
            glClearDepth(desc.clearDepthValue);
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
        glDrawArrays(GetTopology(Impl->currentTopology), static_cast<GLint>(StartVertex), static_cast<GLsizei>(VertexCount));
    }

    void RenderDevice::DrawIndexed(u32 IndexCount, u32 StartIndex, s32 BaseVertex) {
        glDrawElementsBaseVertex(GetTopology(Impl->currentTopology),
                                 static_cast<GLsizei>(IndexCount),
                                 GL_UNSIGNED_INT,
                                 reinterpret_cast<const void*>(static_cast<uintptr_t>(StartIndex * sizeof(u32))),
                                 static_cast<GLint>(BaseVertex));
    }

}