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

typedef void(*GameInitFn)(Hx::Context* context);
typedef void(*GameShutdownFn)();
typedef void(*GameTickFn)(float deltaTime);

class CameraController {
public:
    CameraController(Hx::Camera* inCamera);

    void Tick(f32 deltaTime);

private:
    Hx::Camera* camera;
    Hx::Vector3 velocity;
    f32 mouseSensitivity;
    f32 friction;
    f32 speed;
};

CameraController::CameraController(Hx::Camera* inCamera) {
    camera = inCamera;
    velocity = Hx::Vector3::Zero();
    mouseSensitivity = 0.1f;
    friction = 0.9f;
    speed = 32.0f;
}

void CameraController::Tick(f32 deltaTime) {
    Hx::Vector3 camForward = camera->GetForwardVector();
    Hx::Vector3 camRight = camera->GetRightVector();

    const bool* keyboard = SDL_GetKeyboardState(NULL);

    if (keyboard[SDL_SCANCODE_A]) {
        velocity -= camRight * speed * deltaTime;
    }

    if (keyboard[SDL_SCANCODE_D]) {
        velocity += camRight * speed * deltaTime;
    }

    if (keyboard[SDL_SCANCODE_W]) {
        velocity += camForward * speed * deltaTime;
    }

    if (keyboard[SDL_SCANCODE_S]) {
        velocity -= camForward * speed * deltaTime;
    }

    if (keyboard[SDL_SCANCODE_Q]) {
        velocity.y -= speed * deltaTime;
    }

    if (keyboard[SDL_SCANCODE_E]) {
        velocity.y += speed * deltaTime;
    }

    camera->position += velocity * deltaTime;
    velocity *= friction;

    if (Hx::Abs(velocity.x) < 1e-3f) velocity.x = 0.0f;
    if (Hx::Abs(velocity.y) < 1e-3f) velocity.y = 0.0f;

    f32 mouseDX, mouseDY;
    Uint32 mouseState = SDL_GetRelativeMouseState(&mouseDX, &mouseDY);
    
    camera->rotation.x -= mouseDY * mouseSensitivity;
    camera->rotation.y += mouseDX * mouseSensitivity;
}

int main(int argCount, char** argValues) {
    SDL_Init(SDL_INIT_VIDEO);

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    SDL_Window* window = SDL_CreateWindow("HARM", 800, 600, SDL_WINDOW_OPENGL);
    SDL_GLContext glContext = SDL_GL_CreateContext(window);
    SDL_GL_MakeCurrent(window, glContext);

    SDL_GL_SetSwapInterval(1);
    SDL_SetWindowRelativeMouseMode(window, true);

    // Load Game DLL
    HMODULE gameDLL = LoadLibraryA("Game.dll");
    if (!gameDLL) {
        SDL_Log("Failed to load Game.dll");
        return -1;
    }

    // Get function pointers from Game DLL
    GameInitFn gameInit = (GameInitFn)GetProcAddress(gameDLL, "GameInit");
    GameShutdownFn gameShutdown = (GameShutdownFn)GetProcAddress(gameDLL, "GameShutdown");
    GameTickFn gameTick = (GameTickFn)GetProcAddress(gameDLL, "GameTick");

    if (!gameInit || !gameShutdown || !gameTick) {
        SDL_Log("Failed to get CreateGameInstance or DestroyGameInstance function from Game.dll");
        FreeLibrary(gameDLL);
        return -1;
    }

    // TODO: Add a name to the allocator
    void* mainMemory = malloc(Hx::Megabytes(16));
    Hx::ArenaAllocator mainArena = {};
    Hx::InitArena(mainArena, mainMemory, Hx::Megabytes(16));

    void* transientMemory = malloc(Hx::Megabytes(8));
    Hx::ArenaAllocator transientArena = {};
    Hx::InitArena(transientArena, transientMemory, Hx::Megabytes(8));

    Hx::FileSystem fileSystem;
    
    // Initialize the render device
    Hx::RenderDeviceDesc renderDeviceDesc = {};
    renderDeviceDesc.width = 800;
    renderDeviceDesc.height = 600;
    renderDeviceDesc.vSync = true;
    renderDeviceDesc.debugLayer = true;

    void* renderDeviceMemory = Hx::Alloc(
            &mainArena.base,
            sizeof(Hx::RenderDevice),
            alignof(Hx::RenderDevice),
            Hx::AllocFlags::ZeroInit
    );

    Hx::RenderDevice* renderDevice = new (renderDeviceMemory) Hx::RenderDevice(renderDeviceDesc);

    // Initialize the render system
    void* renderSystemMemory = Hx::Alloc(
            &mainArena.base,
            sizeof(Hx::RenderSystem),
            alignof(Hx::RenderSystem),
            Hx::AllocFlags::ZeroInit
    );
    
    Hx::RenderSystem* renderSystem = new (renderSystemMemory) Hx::RenderSystem(renderDevice);

    Hx::Context engineContext = {};
    engineContext.fileSystem = &fileSystem;
    engineContext.mainArena = &mainArena;
    engineContext.transientArena = &transientArena;

    // Initialize the game
    gameInit(&engineContext);

    Hx::Vertex vertices[] = {
        { { -0.5f, -0.5f, 0.0f }, { 0.0f, 0.0f, 1.0f }, { 0.0f, 0.0f } },
        { {  0.5f, -0.5f, 0.0f }, { 0.0f, 0.0f, 1.0f }, { 1.0f, 0.0f } },
        { {  0.0f,  0.5f, 0.0f }, { 0.0f, 0.0f, 1.0f }, { 0.5f, 1.0f } }
    };

    uint32_t indices[] = { 0, 1, 2 };

    Hx::MeshHandle mesh = renderSystem->CreateMesh(vertices, 3, indices, 3);
    Hx::MaterialHandle material = renderSystem->CreateMaterial(Hx::MaterialType::Opaque);

    Hx::Camera camera;
    camera.SetPerspective(70.0f, 800.0f / 600.0f, 0.1f, 100.0f);
    camera.position = Hx::Vector3(0.0f, 0.0f, 3.0f);

    CameraController camController(&camera);

    Hx::MapData* map = Hx::LoadMapFromFile("Maps/TestMap.map", fileSystem, transientArena);

    bool running = true;
    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                running = false;
            }
        }

        static Uint64 lastTime = SDL_GetPerformanceCounter();
        Uint64 currentTime = SDL_GetPerformanceCounter();
        f32 deltaTime = static_cast<f32>(currentTime - lastTime) / static_cast<f32>(SDL_GetPerformanceFrequency());
        lastTime = currentTime;

        gameTick(deltaTime);

        camController.Tick(deltaTime);

        Hx::Matrix4 projectionMatrix = camera.GetProjectionMatrix();
        Hx::Matrix4 viewMatrix = camera.GetViewMatrix();
        Hx::Matrix4 modelMatrix = Hx::Matrix4::Identity();

        renderSystem->BeginFrame(viewMatrix, projectionMatrix);
        renderSystem->Submit(mesh, material, modelMatrix);
        renderSystem->EndFrame();

        SDL_GL_SwapWindow(window);
    }

    gameShutdown();
    if (gameDLL) {
        FreeLibrary(gameDLL);
    }

    SDL_GL_DestroyContext(glContext);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
