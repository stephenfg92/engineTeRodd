#pragma once

class Game {
    private:
        bool estaRodando;

    public:
        Game();
        ~Game();
        void Inicializar();
        void Executar();
        void Destruir();
        void Desenhar();
};