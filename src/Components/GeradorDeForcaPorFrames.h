#pragma once

#include <raylib.h>

struct GeradorDeForcaPorFramesComponent{
    Vector2 forca;
    int duracaoEmFrames;

    GeradorDeForcaPorFramesComponent(
        Vector2 forca,
        int duracaoEmFrames
    ) {
        this->forca = forca;
        this->duracaoEmFrames = duracaoEmFrames;
    }
};