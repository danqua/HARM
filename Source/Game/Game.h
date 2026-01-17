#pragma once

namespace Engine {
    struct Context;
};

#ifdef GAME_EXPORTS
    #define GAME_API __declspec(dllexport)
#else
    #define GAME_API __declspec(dllimport)
#endif

extern "C" {

    GAME_API void GameInit(Engine::Context* EngineContext);
    GAME_API void GameShutdown();
    GAME_API void GameTick(float DeltaTime);

}