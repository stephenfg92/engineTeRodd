#pragma once
#include <Raylib.h>
#include "../Game/Game.h"
#include "../Ut/ut.h"

struct TransformComponent {
    Vector2 position;
    Vector2 scale;
    double rotation;

    TransformComponent(Vector2 position = Vector2{0, 0}, Vector2 scale = Vector2{Game::scaleFactor, Game::scaleFactor}, double rotation = 0.0) {
        this->position = position;
        this->scale = scale;
        this->rotation = rotation;
    }
};