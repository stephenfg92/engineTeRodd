#include <fstream>

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

    larguraTela = 1600;
    alturaTela = 900;

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


void Game::InicializarJogo(){
    CarregarNivel();
}

void Game::CarregarNivel() {
    registry->AddSystem<KeyboardControlSystem>();
    registry->AddSystem<ProjectileEmitterSystem>();
    registry->AddSystem<AnimationSystem>();
    registry->AddSystem<RenderSystem>();
    registry->AddSystem<MovementSystem>();

    assetStore->AddTexture("tank-image", "assets/images/tank-panther-right.png");
    assetStore->AddTexture("chopper-image", "assets/images/chopper-spritesheet.png");

    // Load tilemap
    int tileSize = 32;
    float tileScale = scaleFactor;
    int numCols = 25;
    int numRows = 20;
    std::fstream mapFile;
    mapFile.open("assets/tilemaps/jungle.map");

    for (int y = 0; y < numRows; y++) {
        for (int x = 0; x < numCols; x++) {
            char ch;
            // primeiro digito
            mapFile.get(ch);
            int srcRectY = std::atoi(&ch) * tileSize; // atoi = Ascii to Integer. Parses the char into an Int.
            // segundo digito
            mapFile.get(ch);
            int srcRectX = std::atoi(&ch) * tileSize;
            // ignora a vírgula
            mapFile.ignore();

            Entity tile = registry->CreateEntity();

            tile.AddTag(Tag::TILE);
            tile.AddComponent<TransformComponent>(
                Vector2{x * (tileScale * tileSize),  y * (tileScale * tileSize)}, 
                Vector2{tileScale, tileScale}, 
                0.0);
            tile.AddComponent<SpriteComponent>("tilemap-image", TILEMAP_LAYER ,tileSize, tileSize, false, false, srcRectX, srcRectY);        
        }
    }
    mapFile.close();

    mapWidth = numCols * tileSize * tileScale;
    mapHeight = numRows * tileSize * tileScale;

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