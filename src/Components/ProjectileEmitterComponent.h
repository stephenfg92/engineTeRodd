#pragma once

#include <raylib.h>

#include "../ECS/ECS.h"

struct ProjectileEmitterComponent {
    int targetEntityId;
    Vector2 projectileVelocity;
    int repeatFrequency;
    int projectileDuration;
    int hitPercentDamage;
    bool isFriendly;
    int lastEmissionTime;
    bool emissionRequestedByEvent;

    ProjectileEmitterComponent(
        Vector2 projectileVelocity = Vector2{.0f, .0f},
        int repeatFrequency = 0,
        int projectileDuration = 10000,
        int hitPercentDamage = 10,
        bool isFriendly = false
    ) {
        this->projectileVelocity = projectileVelocity;
        this->repeatFrequency = repeatFrequency;
        this->projectileDuration = projectileDuration;
        this->hitPercentDamage = hitPercentDamage;
        this->isFriendly = isFriendly;
        this->lastEmissionTime = GetTime();
        this->emissionRequestedByEvent = false;
    }

    ProjectileEmitterComponent(
        int targetEntityId,
        Vector2 projectileVelocity = Vector2{.0f, .0f},
        int repeatFrequency = 0,
        int projectileDuration = 10000,
        int hitPercentDamage = 10,
        bool isFriendly = false
    ) {
        this->targetEntityId = targetEntityId;
        this->projectileVelocity = projectileVelocity;
        this->repeatFrequency = repeatFrequency;
        this->projectileDuration = projectileDuration;
        this->hitPercentDamage = hitPercentDamage;
        this->isFriendly = isFriendly;
        this->lastEmissionTime = GetTime();
        this->emissionRequestedByEvent = false;
    }
};