#pragma once

#include <raylib.h>
#include "../ECS/ECS.h"
#include "../AssetStore/AssetStore.h"

class Game {
    private:
        bool estaRodando;
        Rectangle camera;

        std::unique_ptr<Registry> registry;
        std::unique_ptr<AssetStore> assetStore;

    public:
        static constexpr double scaleFactor = 3.0;

        Game();
        ~Game();
        void Inicializar();
        void Executar();
        void Destruir();
        void Atualizar();
        void Desenhar();
        void CarregarNivel();
        void InicializarJogo();
};