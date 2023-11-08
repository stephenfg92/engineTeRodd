#pragma once

#include <raylib.h>

#include "../ECS/ECS.h"
#include "../Components/ParticleComponent.h"

struct GeradorDeForcaDeMola{
    Entity entidadeNaOutraExtremidade;
    float constanteDaMola;
    float comprimentoDeRepouso;

    GeradorDeForcaDeMola(
        Entity entidadeNaOutraExtremidade,
        float constanteDaMola,
        float comprimentoDeRepouso
    ) {
        this->idEntidadeNaOutraExtremidade = idEntidadeNaOutraExtremidade;
        this->constanteDaMola = constanteDaMola;
        this->comprimentoDeRepouso = comprimentoDeRepouso;
    }
};