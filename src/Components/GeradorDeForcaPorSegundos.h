#pragma once

#include <raylib.h>

struct GeradorDeForcaPorSegundosComponent{
    Vector2 forca;
    double duracaoEmSegundos;
    double tempoInicial;

    GeradorDeForcaPorSegundosComponent(
        Vector2 forca,
        double duracaoEmSegundos
    ) {
        this->forca = forca;
        this->duracaoEmSegundos = duracaoEmSegundos;
        this->tempoInicial = GetTime();
    }
};