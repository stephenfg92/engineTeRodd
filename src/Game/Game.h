#pragma once

#include <raylib.h>
#include "../ECS/ECS.h"
#include "../AssetStore/AssetStore.h"
#include "../Events/EventBus.h"

class Game {
    private:
        bool estaRodando;
        Rectangle camera;

        std::unique_ptr<Registry> registry;
        std::unique_ptr<AssetStore> assetStore;
        std::unique_ptr<EventBus> eventBus;

    public:
        static constexpr float scaleFactor = 3.0f;
        static constexpr int targetFps = 60;
        int mapWidth;
        int mapHeight;
        int larguraTela;
        int alturaTela;

        Game();
        ~Game();
        void Inicializar();
        void Executar();
        void Destruir();
        void Atualizar();
        void Desenhar();
        void CarregarNivel();
        void InicializarJogo();
        void ProcessarComandos();
};