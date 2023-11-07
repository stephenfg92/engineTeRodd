#pragma once

#include <raylib.h>

struct ProjectileComponent {
    bool isFriendly;
    double duration;
    double startTime;

    ProjectileComponent(
        bool isFriendly = false,
        int duration = 10
    ) {
        this->isFriendly = isFriendly;
        this->duration = duration;
        this->startTime = GetTime();
    }
};