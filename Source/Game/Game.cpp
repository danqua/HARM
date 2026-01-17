#include "Game.h"
#include "Engine/Engine.h"
#include <iostream>

using Engine::usize;

class Game {
public:
    Game(Engine::Context* InEngineContext);

    void Initialize();
    void Shutdown();
    void Tick(float DeltaTime);

private:
    Engine::Context* Engine;
};

Game::Game(Engine::Context* InEngineContext) {
    Engine = InEngineContext;
}

void Game::Initialize() {
    std::cout << "Initialize Game" << std::endl;

    auto FileSystem = Engine->FileSystem;
    auto FileHandle = FileSystem->OpenFileWrite("Test.txt");
    const char* Message = "Hello from Game Module!\n";
    usize MessageSizeInBytes = strlen(Message);
    FileHandle->Write(Message, MessageSizeInBytes);
    FileSystem->CloseFile(FileHandle);
}

void Game::Shutdown() {
    std::cout << "Shutdown Game" << std::endl;
}

void Game::Tick(float DeltaTime) {
}

static Game* GGameInstance = nullptr;

extern "C" {

    GAME_API void GameInit(Engine::Context* EngineContext) {
        if (GGameInstance) {
            GGameInstance->Shutdown();
        }

        GGameInstance = new Game(EngineContext);
        GGameInstance->Initialize();
    }

    GAME_API void GameShutdown() {
        if (GGameInstance) {
            GGameInstance->Shutdown();
            delete GGameInstance;
            GGameInstance = nullptr;
        }
    }

    GAME_API void GameTick(float DeltaTime) {
        if (GGameInstance) {
            GGameInstance->Tick(DeltaTime);
        }
    }

}