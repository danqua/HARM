#include <SDL3/SDL.h>

#include "Engine/RenderCore/RenderDevice.h"
#include "Engine/Renderer/RenderSystem.h"
#include "Engine/Math/Math.h"

#include "Engine/Memory/ArenaAllocator.h"

int main(int ArgCount, char** ArgValues) {
    SDL_Init(SDL_INIT_VIDEO);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    SDL_Window* Window = SDL_CreateWindow("HARM", 800, 600, SDL_WINDOW_OPENGL);
    SDL_GLContext GLContext = SDL_GL_CreateContext(Window);
    SDL_GL_MakeCurrent(Window, GLContext);

    // TODO: Add a name to the allocator
    void* MainMemory = malloc(Engine::Memory::Megabytes(16));
    Engine::Memory::ArenaAllocator MainArena = {};
    Engine::Memory::InitArena(MainArena, MainMemory, Engine::Memory::Megabytes(16));
    
    // Initialize the render device
    Engine::RenderCore::RenderDeviceDesc RenderDeviceDesc = {};
    RenderDeviceDesc.Width = 800;
    RenderDeviceDesc.Height = 600;
    RenderDeviceDesc.VSync = true;
    RenderDeviceDesc.DebugLayer = true;

    void* RenderDeviceMemory = Engine::Memory::Alloc(
            &MainArena.Base,
            sizeof(Engine::RenderCore::RenderDevice),
            alignof(Engine::RenderCore::RenderDevice),
            Engine::Memory::AllocFlags::ZeroInit
    );

    Engine::RenderCore::RenderDevice* RenderDevice = new (RenderDeviceMemory) Engine::RenderCore::RenderDevice(RenderDeviceDesc);

    // Initialize the render system
    void* RenderSystemMemory = Engine::Memory::Alloc(
            &MainArena.Base,
            sizeof(Engine::Renderer::RenderSystem),
            alignof(Engine::Renderer::RenderSystem),
            Engine::Memory::AllocFlags::ZeroInit
    );
    
    Engine::Renderer::RenderSystem* RenderSystem = new (RenderSystemMemory) Engine::Renderer::RenderSystem(RenderDevice);

    Engine::Renderer::Vertex Vertices[] = {
        { { -0.5f, -0.5f, 0.0f }, { 0.0f, 0.0f, 1.0f }, { 0.0f, 0.0f } },
        { {  0.5f, -0.5f, 0.0f }, { 0.0f, 0.0f, 1.0f }, { 1.0f, 0.0f } },
        { {  0.0f,  0.5f, 0.0f }, { 0.0f, 0.0f, 1.0f }, { 0.5f, 1.0f } }
    };

    uint32_t Indices[] = { 0, 1, 2 };

    Engine::Renderer::MeshHandle Mesh = RenderSystem->CreateMesh(Vertices, 3, Indices, 3);
    Engine::Renderer::MaterialHandle Material = RenderSystem->CreateMaterial(Engine::Renderer::MaterialType::Opaque);

    Engine::Math::Matrix4 ViewMatrix = Engine::Math::LookAt(
        Engine::Math::Vector3(0.0f, 0.0f, 3.0f),
        Engine::Math::Vector3(0.0f, 0.0f, 0.0f),
        Engine::Math::Vector3(0.0f, 1.0f, 0.0f)
    );

    Engine::Math::Matrix4 ProjectionMatrix = Engine::Math::Perspective(
        Engine::Math::Radians(70.0f),
        800.0f / 600.0f,
        0.1f, 100.0f
    );

    bool Running = true;
    while (Running) {
        SDL_Event Event;
        while (SDL_PollEvent(&Event)) {
            if (Event.type == SDL_EVENT_QUIT) {
                Running = false;
            }
        }

        Engine::Math::Matrix4 ModelMatrix = Engine::Math::Identity();

        RenderSystem->BeginFrame(ViewMatrix, ProjectionMatrix);
        RenderSystem->Submit(Mesh, Material, ModelMatrix);
        RenderSystem->EndFrame();

        SDL_GL_SwapWindow(Window);
    }

    SDL_GL_DestroyContext(GLContext);
    SDL_DestroyWindow(Window);
    SDL_Quit();
    return 0;
}