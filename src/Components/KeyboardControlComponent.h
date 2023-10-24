#pragma once

#include <raylib.h>

struct KeyboardControlComponent {
    Vector2 upVelocity;
    Vector2 rightVelocity;
    Vector2 downVelocity;
    Vector2 leftVelocity;

    KeyboardControlComponent(
        Vector2 upVelocity = Vector2{0.0f, 0.0f},
        Vector2 rightVelocity = Vector2{0.0f, 0.0f},
        Vector2 downVelocity = Vector2{0.0f, 0.0f},
        Vector2 leftVelocity = Vector2{0.0f, 0.0f}
    ) {
        this->upVelocity = upVelocity;
        this->rightVelocity = rightVelocity;
        this->downVelocity = downVelocity;
        this->leftVelocity = leftVelocity;
    }
};