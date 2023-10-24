#include "Game.h"

#include "../Events/EventBus.h"

#include "../Systems/RenderSystem.h"
#include "../Systems/AnimationSystem.h"
#include "../Systems/KeyboardControlSystem.h"
#include "../Systems/ProjectileEmitterSystem.h"
#include "../Systems/MovementSystem.h"

#include "../Components/TransformComponent.h"
#include "../Components/SpriteComponent.h"
#include "../Components/AnimationComponent.h"
#include "../Components/RigidBodyComponent.h"
#include "../Components/KeyboardControlComponent.h"

Game::Game() {
    estaRodando = false;
}

Game::~Game() {

}

void Game::Destruir(){
    CloseWindow();
}

void Game::Inicializar(){
    registry = std::make_unique<Registry>();
    assetStore = std::make_unique<AssetStore>();
    eventBus = std::make_unique<EventBus>();

    const int larguraTela = 1600;
    const int alturaTela = 900;

    camera.x = 0;
    camera.y = 0;
    camera.width = larguraTela;
    camera.height = alturaTela;

    InitWindow(larguraTela, alturaTela, "engine");

    SetTargetFPS(targetFps);

    estaRodando = true;
}

void Game::Executar(){
    InicializarJogo();
    while(estaRodando && !WindowShouldClose()){
        ProcessarComandos();
        Atualizar();
        Desenhar();
    }
}

void Game::ProcessarComandos(){
    registry->GetSystem<KeyboardControlSystem>().Update(eventBus);
}

void Game::Atualizar(){
    eventBus->Reset();

    registry->GetSystem<ProjectileEmitterSystem>().SubscribeToEvents(eventBus);
    registry->GetSystem<MovementSystem>().SubscribeToEvents(eventBus);

    registry->GetSystem<AnimationSystem>().Update();
    registry->GetSystem<ProjectileEmitterSystem>().Update(registry);
    registry->GetSystem<MovementSystem>().Update(GetFrameTime());

    registry->Update();
}

void Game::Desenhar(){
    BeginDrawing();

        ClearBackground(RAYWHITE);

        registry->GetSystem<RenderSystem>().Update(camera, assetStore);
        DrawText("RenderSystem!", 240, 350, 10, BLACK);

    EndDrawing();
}

void Game::CarregarNivel() {
    registry->AddSystem<KeyboardControlSystem>();
    registry->AddSystem<ProjectileEmitterSystem>();
    registry->AddSystem<AnimationSystem>();
    registry->AddSystem<RenderSystem>();
    registry->AddSystem<MovementSystem>();

    assetStore->AddTexture("tank-image", "assets/images/tank-panther-right.png");
    assetStore->AddTexture("chopper-image", "assets/images/chopper-spritesheet.png");

    Entity tank = registry->CreateEntity();
    tank.AddComponent<TransformComponent>(Vector2{300.0, 700.0});
    tank.AddComponent<SpriteComponent>("tank-image", LAYER_1, 32, 32);

    Entity chopper = registry->CreateEntity();
    chopper.AddComponent<TransformComponent>(Vector2{800.0, 350.0});
    chopper.AddComponent<SpriteComponent>("chopper-image", LAYER_2, 32, 32);
    chopper.AddComponent<AnimationComponent>(2, 10);
    chopper.AddComponent<RigidBodyComponent>();
    chopper.AddComponent<KeyboardControlComponent>(Vector2{.0f, -300.0f}, Vector2{300.f, .0f}, Vector2{.0f, 300.0f}, Vector2{-300.0f, .0f});
}

void Game::InicializarJogo(){
    CarregarNivel();
}