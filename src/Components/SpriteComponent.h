#pragma once

#include<string>
#include<Raylib.h>

struct SpriteComponent {
    std::string assetId;
    unsigned int zIndex;
    int width;
    int height;
    bool isFixed;
    bool flipX;
    bool flipY;
    Rectangle srcRect;

    SpriteComponent(
        std::string assetId = "",
        unsigned int zIndex = 0,
        float width = 1,
        float height = 1,
        bool isFixed = false,
        bool flipX = false,
        bool flipY = false,
        float srcRectX = 0,
        float srcRectY = 0
    ) {
        this->assetId = assetId;
        this->zIndex = zIndex;
        this->width = width;
        this->height = height;
        this->isFixed = isFixed;
        this->flipX = flipX;
        this->flipY = flipY;
        this->srcRect = { srcRectX, srcRectY, width, height };       
    }
};

enum zIndex {TILEMAP_LAYER = 0, LAYER_1 = 1, LAYER_2 = 2};