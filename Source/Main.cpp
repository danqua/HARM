#include <SDL3/SDL.h>

#include "Engine/IO/FileSystem.h"
#include "Engine/RenderCore/RenderDevice.h"
#include "Engine/Renderer/RenderSystem.h"
#include "Engine/Renderer/Camera.h"
#include "Engine/Math/Math.h"

#include "Engine/Memory/ArenaAllocator.h"
#include "Engine/World/Level/MapData.h"
#include "Engine/Engine.h"

#include <memory>
#include <array>

#include <windows.h>

typedef void(*GameInitFn)(Engine::Context* Context);
typedef void(*GameShutdownFn)();
typedef void(*GameTickFn)(float DeltaTime);

using Engine::f32;
using Engine::Math::Vector3;

class CameraController {
public:
    CameraController(Engine::Renderer::Camera* InCamera);

    void Tick(f32 DeltaTime);

private:
    Engine::Renderer::Camera* Camera;
    Vector3 Velocity;
    f32 MouseSensitivity;
    f32 Friction;
    f32 Speed;
};

CameraController::CameraController(Engine::Renderer::Camera* InCamera) {
    Camera = InCamera;
    Velocity = Vector3::Zero();
    MouseSensitivity = 0.1f;
    Friction = 0.9f;
    Speed = 32.0f;
}

void CameraController::Tick(f32 DeltaTime) {
    Vector3 CamForward = Camera->GetForwardVector();
    Vector3 CamRight = Camera->GetRightVector();

    const bool* Keyboard = SDL_GetKeyboardState(NULL);

    if (Keyboard[SDL_SCANCODE_A]) {
        Velocity -= CamRight * Speed * DeltaTime;
    }

    if (Keyboard[SDL_SCANCODE_D]) {
        Velocity += CamRight * Speed * DeltaTime;
    }

    if (Keyboard[SDL_SCANCODE_W]) {
        Velocity += CamForward * Speed * DeltaTime;
    }

    if (Keyboard[SDL_SCANCODE_S]) {
        Velocity -= CamForward * Speed * DeltaTime;
    }

    if (Keyboard[SDL_SCANCODE_Q]) {
        Velocity.y -= Speed * DeltaTime;
    }

    if (Keyboard[SDL_SCANCODE_E]) {
        Velocity.y += Speed * DeltaTime;
    }

    Camera->Position += Velocity * DeltaTime;
    Velocity *= Friction;

    if (Engine::Math::Abs(Velocity.x) < 1e-3f) Velocity.x = 0.0f;
    if (Engine::Math::Abs(Velocity.y) < 1e-3f) Velocity.y = 0.0f;

    f32 MouseDX, MouseDY;
    Uint32 MouseState = SDL_GetRelativeMouseState(&MouseDX, &MouseDY);
    
    Camera->Rotation.x -= MouseDY * MouseSensitivity;
    Camera->Rotation.y += MouseDX * MouseSensitivity;
}

int main(int ArgCount, char** ArgValues) {
    SDL_Init(SDL_INIT_VIDEO);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    SDL_Window* Window = SDL_CreateWindow("HARM", 800, 600, SDL_WINDOW_OPENGL);
    SDL_GLContext GLContext = SDL_GL_CreateContext(Window);
    SDL_GL_MakeCurrent(Window, GLContext);

    SDL_GL_SetSwapInterval(1);
    SDL_SetWindowRelativeMouseMode(Window, true);

    // Load Game DLL
    HMODULE GameDLL = LoadLibraryA("Game.dll");
    if (!GameDLL) {
        SDL_Log("Failed to load Game.dll");
        return -1;
    }

    // Get function pointers from Game DLL
    GameInitFn GameInit = (GameInitFn)GetProcAddress(GameDLL, "GameInit");
    GameShutdownFn GameShutdown = (GameShutdownFn)GetProcAddress(GameDLL, "GameShutdown");
    GameTickFn GameTick = (GameTickFn)GetProcAddress(GameDLL, "GameTick");

    if (!GameInit || !GameShutdown || !GameTick) {
        SDL_Log("Failed to get CreateGameInstance or DestroyGameInstance function from Game.dll");
        FreeLibrary(GameDLL);
        return -1;
    }

    // TODO: Add a name to the allocator
    void* MainMemory = malloc(Engine::Memory::Megabytes(16));
    Engine::Memory::ArenaAllocator MainArena = {};
    Engine::Memory::InitArena(MainArena, MainMemory, Engine::Memory::Megabytes(16));

    void* TransientMemory = malloc(Engine::Memory::Megabytes(8));
    Engine::Memory::ArenaAllocator TransientArena = {};
    Engine::Memory::InitArena(TransientArena, TransientMemory, Engine::Memory::Megabytes(8));

    Engine::IO::FileSystem FileSystem;
    
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

    Engine::Context EngineContext = {};
    EngineContext.FileSystem = &FileSystem;
    EngineContext.MainArena = &MainArena;
    EngineContext.TransientArena = &TransientArena;

    // Initialize the game
    GameInit(&EngineContext);

    Engine::Renderer::Vertex Vertices[] = {
        { { -0.5f, -0.5f, 0.0f }, { 0.0f, 0.0f, 1.0f }, { 0.0f, 0.0f } },
        { {  0.5f, -0.5f, 0.0f }, { 0.0f, 0.0f, 1.0f }, { 1.0f, 0.0f } },
        { {  0.0f,  0.5f, 0.0f }, { 0.0f, 0.0f, 1.0f }, { 0.5f, 1.0f } }
    };

    uint32_t Indices[] = { 0, 1, 2 };

    Engine::Renderer::MeshHandle Mesh = RenderSystem->CreateMesh(Vertices, 3, Indices, 3);
    Engine::Renderer::MaterialHandle Material = RenderSystem->CreateMaterial(Engine::Renderer::MaterialType::Opaque);

    Engine::Renderer::Camera Camera;
    Camera.SetPerspective(70.0f, 800.0f / 600.0f, 0.1f, 100.0f);
    Camera.Position = Engine::Math::Vector3(0.0f, 0.0f, 3.0f);

    CameraController CamController(&Camera);

    Engine::World::MapData* Map = Engine::World::LoadMapFromFile("Maps/TestMap.map", FileSystem, TransientArena);

    bool Running = true;
    while (Running) {
        SDL_Event Event;
        while (SDL_PollEvent(&Event)) {
            if (Event.type == SDL_EVENT_QUIT) {
                Running = false;
            }
        }

        static Uint64 LastTime = SDL_GetPerformanceCounter();
        Uint64 CurrentTime = SDL_GetPerformanceCounter();
        f32 DeltaTime = static_cast<f32>(CurrentTime - LastTime) / static_cast<f32>(SDL_GetPerformanceFrequency());
        LastTime = CurrentTime;

        GameTick(DeltaTime);

        CamController.Tick(DeltaTime);

        Engine::Math::Matrix4 ProjectionMatrix = Camera.GetProjectionMatrix();
        Engine::Math::Matrix4 ViewMatrix = Camera.GetViewMatrix();
        Engine::Math::Matrix4 ModelMatrix = Engine::Math::Matrix4::Identity();

        RenderSystem->BeginFrame(ViewMatrix, ProjectionMatrix);
        RenderSystem->Submit(Mesh, Material, ModelMatrix);
        RenderSystem->EndFrame();

        SDL_GL_SwapWindow(Window);
    }

    GameShutdown();
    if (GameDLL) {
        FreeLibrary(GameDLL);
    }

    SDL_GL_DestroyContext(GLContext);
    SDL_DestroyWindow(Window);
    SDL_Quit();
    return 0;
}