#include "Game.h"

#include "../Systems/RenderSystem.h"
#include "../Systems/AnimationSystem.h"

#include "../Components/TransformComponent.h"
#include "../Components/SpriteComponent.h"
#include "../Components/AnimationComponent.h"

Game::Game() {
    estaRodando = false;
}

Game::~Game() {

}


void Game::Inicializar(){
    registry = std::make_unique<Registry>();
    assetStore = std::make_unique<AssetStore>();

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
        //ProcessarInput()
        Atualizar();
        Desenhar();
    }
}

void Game::Destruir(){
    CloseWindow();
}

void Game::Atualizar(){
    registry->GetSystem<AnimationSystem>().Update();

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
    registry->AddSystem<AnimationSystem>();
    registry->AddSystem<RenderSystem>();

    assetStore->AddTexture("tank-image", "assets/images/tank-panther-right.png");
    assetStore->AddTexture("chopper-image", "assets/images/chopper-spritesheet.png");

    Entity tank = registry->CreateEntity();
    tank.AddComponent<TransformComponent>(Vector2{300.0, 700.0});
    tank.AddComponent<SpriteComponent>("tank-image", LAYER_1, 32, 32);

    Entity chopper = registry->CreateEntity();
    chopper.AddComponent<TransformComponent>(Vector2{500.0, 700.0});
    chopper.AddComponent<SpriteComponent>("chopper-image", LAYER_2, 32, 32);
    chopper.AddComponent<AnimationComponent>(2, 10);
}

void Game::InicializarJogo(){
    CarregarNivel();
}