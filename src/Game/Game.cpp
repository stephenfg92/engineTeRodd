#include <raylib.h>
#include "Game.h"

Game::Game() {
    estaRodando = false;
}

Game::~Game() {

}


void Game::Inicializar(){
    const int larguraTela = 1600;
    const int alturaTela = 900;

    InitWindow(larguraTela, alturaTela, "engine");

    SetTargetFPS(60);

    estaRodando = true;
}

void Game::Executar(){
    while(estaRodando && !WindowShouldClose()){
        Desenhar();
    }
}

void Game::Destruir(){
    CloseWindow();
}

void Game::Desenhar(){
    BeginDrawing();

        ClearBackground(RAYWHITE);

        DrawText("Janela básica!", 190, 200, 20, LIGHTGRAY);

    EndDrawing();
}