#pragma once

#include <raylib.h>

struct ProjectileComponent {
    bool isFriendly;
    int duration;
    int startTime;

    ProjectileComponent(
        bool isFriendly = false,
        int duration = 1000
    ) {
        this->isFriendly = isFriendly;
        this->duration = duration;
        this->startTime = GetTime();
    }
};