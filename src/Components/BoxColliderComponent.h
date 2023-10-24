#pragma once

#include <raylib.h>

struct BoxColliderComponent {
    int width;
    int height;
    Vector2 offset;

    BoxColliderComponent(int width = 0, int height = 0, Vector2 offset = Vector2{.0f, .0f}){
        this->height = height;
        this->width = width;
        this->offset = offset;
    }
};