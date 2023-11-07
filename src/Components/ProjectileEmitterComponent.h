#pragma once

#include <raylib.h>

#include "../ECS/ECS.h"

struct ProjectileEmitterComponent {
    int targetEntityId;
    Vector2 projectileVelocity;
    double repeatFrequency;
    double cooldown;
    double projectileDuration;
    int hitPercentDamage;
    bool isFriendly;
    double lastEmissionTime;
    bool emissionRequestedByEvent;    

    ProjectileEmitterComponent(
        Vector2 projectileVelocity = Vector2{.0f, .0f},
        double repeatFrequency = 0,
        double cooldown = 0,
        double projectileDuration = 10.00,
        int hitPercentDamage = 10,
        bool isFriendly = false
    ) {
        this->projectileVelocity = projectileVelocity;
        this->repeatFrequency = repeatFrequency;
        this->cooldown = cooldown;
        this->projectileDuration = projectileDuration;
        this->hitPercentDamage = hitPercentDamage;
        this->isFriendly = isFriendly;
        this->lastEmissionTime = GetTime();
        this->emissionRequestedByEvent = false;
    }

    ProjectileEmitterComponent(
        int targetEntityId,
        Vector2 projectileVelocity = Vector2{.0f, .0f},
        double repeatFrequency = 0,
        int projectileDuration = 10000,
        int hitPercentDamage = 10,
        bool isFriendly = false
    ) {
        this->targetEntityId = targetEntityId;
        this->projectileVelocity = projectileVelocity;
        this->repeatFrequency = repeatFrequency;
        this->cooldown = 0;
        this->projectileDuration = projectileDuration;
        this->hitPercentDamage = hitPercentDamage;
        this->isFriendly = isFriendly;
        this->lastEmissionTime = GetTime();
        this->emissionRequestedByEvent = false;
    }
};