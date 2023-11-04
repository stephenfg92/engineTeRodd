#include <fstream>

#include "Game.h"

#include "../imgui/imgui.h"
#include "../imgui/imgui_impl_raylib.h"

#include "../ECS/Tag.h"

#include "../Events/EventBus.h"

#include "../Systems/RenderSystem.h"
#include "../Systems/AnimationSystem.h"
#include "../Systems/KeyboardControlSystem.h"
#include "../Systems/ProjectileEmitterSystem.h"
#include "../Systems/MovementSystem.h"
#include "../Systems/ProjectileSystem.h"
#include "../Systems/BoundsCheckingSystem.h"
#include "../Systems/CameraMovementSystem.h"
#include "../Systems/DamageSystem.h"
#include "../Systems/CollisionSystem.h"
#include "../Systems/ParticleSystem.h"
#include "../Systems/RenderEntityInfoSystem.h"
#include "../Systems/RenderColliderSystem.h"
#include "../Systems/RenderTextSystem.h"
#include "../Systems/RenderDbgGuiSystem.h"

#include "../Components/TransformComponent.h"
#include "../Components/SpriteComponent.h"
#include "../Components/AnimationComponent.h"
#include "../Components/RigidBodyComponent.h"
#include "../Components/BoxColliderComponent.h"
#include "../Components/KeyboardControlComponent.h"
#include "../Components/PlayerParticleEmitterComponent.h"

int Game::mapWidth = 0;
int Game::mapHeight = 0;

int Game::larguraTela = 0;
int Game::alturaTela = 0;

Game::Game() {
    estaRodando = false;
    debug = true;
}

Game::~Game() {

}

void Game::Destruir(){
    if (debug){
        ImGui_ImplRaylib_Shutdown();
        ImGui::DestroyContext();
    }

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

    if (debug){
    ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  
        ImGui::StyleColorsDark();
        ImGui_ImplRaylib_Init();
    }

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
    registry->AddSystem<MovementSystem>();
    registry->AddSystem<RenderSystem>();
    registry->AddSystem<AnimationSystem>();
    registry->AddSystem<CollisionSystem>();
    registry->AddSystem<RenderColliderSystem>();
    registry->AddSystem<DamageSystem>();
    registry->AddSystem<KeyboardControlSystem>();
    registry->AddSystem<CameraMovementSystem>();
    registry->AddSystem<ProjectileEmitterSystem>();
    registry->AddSystem<ProjectileSystem>();
    registry->AddSystem<BoundsCheckingSystem>();
    registry->AddSystem<ParticleSystem>();
    registry->AddSystem<RenderTextSystem>();
    registry->AddSystem<RenderEntityInfoSystem>();
    registry->AddSystem<RenderDbgGuiSystem>();

    assetStore->AddTexture("tank-image", "assets/images/tank-panther-right.png");
    assetStore->AddTexture("truck-image", "assets/images/truck-ford-right.png");
    assetStore->AddTexture("chopper-image", "assets/images/chopper-spritesheet.png");
    assetStore->AddTexture("radar-image", "assets/images/radar.png");
    assetStore->AddTexture("bullet-image", "assets/images/bullet.png");
    assetStore->AddTexture("tree-image", "assets/images/tree.png");
    assetStore->AddTexture("tilemap-image", "assets/tilemaps/jungle.png");
    assetStore->AddFont("charriot-font", "assets/fonts/charriot.ttf", 20, 5);

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
            tile.AddComponent<SpriteComponent>("tilemap-image", TILEMAP_LAYER ,tileSize, tileSize, false, false, false, srcRectX, srcRectY);        
        }
    }
    mapFile.close();

    mapWidth = numCols * tileSize * tileScale;
    mapHeight = numRows * tileSize * tileScale;

    Entity chopper = registry->CreateEntity();
    chopper.AddTag(Tag::PLAYER);
    chopper.AddComponent<TransformComponent>(Vector2{800.0, 350.0});
    chopper.AddComponent<RigidBodyComponent>();
    chopper.AddComponent<BoxColliderComponent>(32, 32);
    chopper.AddComponent<SpriteComponent>("chopper-image", LAYER_2, 32, 32);
    chopper.AddComponent<AnimationComponent>(2, 10);
    chopper.AddComponent<KeyboardControlComponent>(Vector2{.0f, -300.0f}, Vector2{300.f, .0f}, Vector2{.0f, 300.0f}, Vector2{-300.0f, .0f});
    chopper.AddComponent<HealthComponent>(100, true, "charriot-font");
    chopper.AddComponent<CameraFollowComponent>();
    chopper.AddComponent<PlayerParticleEmitterComponent>(300.0f, 200.0f, 0.99f, 0.25, 10.0, 25);

    Entity truck = registry->CreateEntity();
    truck.AddTag(Tag::ENEMY);
    truck.AddComponent<TransformComponent>(Vector2{500.0, 40.0});
    truck.AddComponent<SpriteComponent>("truck-image", LAYER_1, 32, 32);
    truck.AddComponent<BoxColliderComponent>(32, 32);
    truck.AddComponent<HealthComponent>(100, true, "charriot-font");

    Entity tank = registry->CreateEntity();
    tank.AddTag(Tag::ENEMY);
    tank.AddComponent<TransformComponent>(Vector2{300.0, 700.0});
    tank.AddComponent<RigidBodyComponent>(Vector2{80.0, 0.0});
    tank.AddComponent<SpriteComponent>("tank-image", LAYER_1, 32, 32);
    tank.AddComponent<BoxColliderComponent>(32, 32);
    tank.AddComponent<ProjectileEmitterComponent>(chopper.GetId(), Vector2{300.0, 0.0}, 0.65, 5000, 25);
    tank.AddComponent<HealthComponent>(100, true, "charriot-font");

    Entity treeA = registry->CreateEntity();
    treeA.AddTag(Tag::OBSTACLE);
    treeA.AddComponent<TransformComponent>(Vector2{630.0, 700.0});
    treeA.AddComponent<SpriteComponent>("tree-image", LAYER_1, 16, 32);
    treeA.AddComponent<BoxColliderComponent>(16, 32);

    Entity treeB = registry->CreateEntity();
    treeB.AddTag(Tag::OBSTACLE);
    treeB.AddComponent<TransformComponent>(Vector2{200.0, 700.0});
    treeB.AddComponent<SpriteComponent>("tree-image", LAYER_1, 16, 32);
    treeB.AddComponent<BoxColliderComponent>(16, 32);

    Entity radar = registry->CreateEntity();
    radar.AddComponent<TransformComponent>(Vector2{static_cast<float>(larguraTela - 74), 10.0f}, Vector2{1.0f, 1.0f});
    radar.AddComponent<SpriteComponent>("radar-image", LAYER_2, 64, 64, true);
    radar.AddComponent<AnimationComponent>(8, 5, true);

    Entity label = registry->CreateEntity();
    label.AddComponent<TextLabelComponent>("charriot-font", "TEXT TEST!", Vector2{100, 100}, RAYWHITE, true);


}

void Game::ProcessarComandos(){
    registry->GetSystem<KeyboardControlSystem>().Update(eventBus);
    ImGui_ImplRaylib_ProcessEvents();
}

void Game::Atualizar(){
    double deltaTempo = GetFrameTime();

    eventBus->Reset();

    // Eventos
    registry->GetSystem<ProjectileEmitterSystem>().SubscribeToEvents(eventBus);
    registry->GetSystem<MovementSystem>().SubscribeToEvents(eventBus);
    registry->GetSystem<DamageSystem>().SubscribeToEvents(eventBus);
    registry->GetSystem<ParticleSystem>().SubscribeToEvents(eventBus);

    // Atualizações
    registry->GetSystem<KeyboardControlSystem>().Update(eventBus);
    registry->GetSystem<ParticleSystem>().Update(eventBus, deltaTempo);
    registry->GetSystem<MovementSystem>().Update(deltaTempo);
    registry->GetSystem<CollisionSystem>().Update(eventBus);
    registry->GetSystem<ProjectileEmitterSystem>().Update(registry);
    registry->GetSystem<ProjectileSystem>().Update();
    registry->GetSystem<AnimationSystem>().Update();
    registry->GetSystem<CameraMovementSystem>().Update(camera);
    registry->GetSystem<BoundsCheckingSystem>().Update(mapWidth, mapHeight);

    registry->Update();
}

void Game::Desenhar(){
    BeginDrawing();

        ClearBackground(RAYWHITE);

        registry->GetSystem<RenderSystem>().Update(camera, assetStore);
        registry->GetSystem<RenderEntityInfoSystem>().Update(camera, assetStore);

        if (debug){
            registry->GetSystem<RenderColliderSystem>().Update(camera);
            registry->GetSystem<RenderDbgGuiSystem>().Update(registry, assetStore, camera);
        }

    EndDrawing();
}