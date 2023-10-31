#pragma once

#include <raylib.h>

struct HealthComponent {
    int healthPercentage;
    bool showHealthPercentage;
    std::string healthPercentageFont;
    int lastHealthPercentage;
    bool showHealthBar;
    Vector2 healthBarSize;
    float lastHealthBarW;

    HealthComponent(
        int healthPercentage = 100, 
        bool showHealthPercentage = true,
        std::string healthPercentageFont = "",
        bool showHealthBar = true,
        Vector2 healthBarSize = Vector2{32, 5}
    ) 
    {
        this->healthPercentage = healthPercentage;
        this->lastHealthPercentage = healthPercentage;
        this->showHealthPercentage = showHealthPercentage;
        this->healthPercentageFont = healthPercentageFont;
        this->showHealthBar = showHealthBar;
        this->healthBarSize = healthBarSize;
        this->lastHealthBarW = healthBarSize.x;
    }
};