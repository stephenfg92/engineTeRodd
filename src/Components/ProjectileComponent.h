#pragma once

#include <raylib.h>

struct ProjectileComponent {
    bool isFriendly;
    int hitPercentDamage;
    int duration;
    int startTime;

    ProjectileComponent(
        bool isFriendly = false,
        int hitPercentDamage = 100,
        int duration = 1000
    ) {
        this->isFriendly = isFriendly;
        this->hitPercentDamage = hitPercentDamage;
        this->duration = duration;
        this->startTime = GetTime();
    }
};