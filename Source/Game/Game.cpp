#include "Game.h"
#include "Engine/Engine.h"
#include <iostream>

class Game {
public:
    Game(Hx::Context* inEngineContext);

    void Initialize();
    void Shutdown();
    void Tick(float deltaTime);

private:
    Hx::Context* engine;
};

Game::Game(Hx::Context* inEngineContext) {
    engine = inEngineContext;
}

void Game::Initialize() {
    std::cout << "Initialize Game" << std::endl;

    auto fileSystem = engine->fileSystem;
    auto fileHandle = fileSystem->OpenFileWrite("Test.txt");
    const char* message = "Hello from Game Module!\n";
    usize messageSizeInBytes = strlen(message);
    fileHandle->Write(message, messageSizeInBytes);
    fileSystem->CloseFile(fileHandle);
}

void Game::Shutdown() {
    std::cout << "Shutdown Game" << std::endl;
}

void Game::Tick(float deltaTime) {
}

static Game* gGameInstance = nullptr;

extern "C" {

    GAME_API void GameInit(Hx::Context* engineContext) {
        if (gGameInstance) {
            gGameInstance->Shutdown();
        }

        gGameInstance = new Game(engineContext);
        gGameInstance->Initialize();
    }

    GAME_API void GameShutdown() {
        if (gGameInstance) {
            gGameInstance->Shutdown();
            delete gGameInstance;
            gGameInstance = nullptr;
        }
    }

    GAME_API void GameTick(float deltaTime) {
        if (gGameInstance) {
            gGameInstance->Tick(deltaTime);
        }
    }

}