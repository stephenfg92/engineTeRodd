#pragma once

#include <raylib.h>

struct RigidBodyComponent {
    Vector2 velocity;

    RigidBodyComponent(Vector2 velocity = Vector2{0.0f, 0.0f}){
        this->velocity = velocity;
    };
};