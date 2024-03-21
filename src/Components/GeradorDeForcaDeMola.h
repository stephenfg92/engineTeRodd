#pragma once

#include <raylib.h>

#include "../ECS/ECS.h"

struct GeradorDeForcaDeMola{
    Entity& entidadeNaOutraExtremidade;
    float constanteDaMola;
    float comprimentoDeRepouso;

    GeradorDeForcaDeMola(
        Entity& entidadeNaOutraExtremidade,
        float constanteDaMola,
        float comprimentoDeRepouso
    ) : entidadeNaOutraExtremidade(entidadeNaOutraExtremidade),
        constanteDaMola(constanteDaMola),
        comprimentoDeRepouso(comprimentoDeRepouso) {
    }
};