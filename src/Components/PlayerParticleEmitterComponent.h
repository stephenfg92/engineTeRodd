#pragma once

#include <raylib.h>

#include "../ECS/ECS.h"

struct PlayerParticleEmitterComponent {
    float rapidez;
    Vector2 sentido;
    float massa;
    float atenuacao;
    double tempoDeEspera;
    double tempoDeVida;
    int danoPercentual;
    double ultimaEmissao;

    PlayerParticleEmitterComponent(
        float rapidez = 5.0f, // 5 m/s
        float massa = 1.0f, // 1kg
        float atenuacao = 0.99f,
        double tempoDeEspera = 0.0, // tempo de espera entre disparos
        double tempoDeVida = 10.0, // 10 s
        int danoPercentual = 10 // 10% de dano
    )
    : rapidez(rapidez),
      massa(massa), 
      atenuacao(atenuacao),
      tempoDeEspera(tempoDeEspera),
      tempoDeVida(tempoDeVida),
      danoPercentual(danoPercentual),
      ultimaEmissao(GetTime())
    {
      sentido = Vector2{1.0f, 0.0f}; // Direita
    }
};