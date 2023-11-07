#pragma once

#include <raylib.h>

struct ParticleComponent {
    Vector2 velocity;
    Vector2 acumuladorDeForca;
    Vector2 aceleracao;
    float massaReciproca;
    float massa;
    float atenuacao;
    double tempoDeVida;
    double tempoDeInicio;
    bool massaInfinita;

    ParticleComponent(
        Vector2 velocity = {50.0f, 0.0f},
        Vector2 aceleracao = Vector2{},
        float massa = 1.0f,
        float atenuacao = 1,
        double tempoDeVida = 30,
        bool massaInfinita = false
    ){
        this->velocity = velocity;
        this->acumuladorDeForca = Vector2{};
        this->aceleracao = aceleracao;
        this->massa = massa;
        this->massaReciproca = 1/massa;
        this->atenuacao = atenuacao;
        this->tempoDeVida = tempoDeVida;
        this->tempoDeInicio = GetTime();
        this->massaInfinita = massaReciproca <= .0f;
    };
};