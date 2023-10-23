#pragma once

#include <raylib.h>

struct AnimationComponent {
    int numFrames;
    int currentFrame;
    int frameRate;
    bool shouldLoop;
    double startTime;

    AnimationComponent(
        int numFrames = 1,
        int frameRate = 1,
        bool shouldLoop = true
    ) {
        this->numFrames = numFrames;
        this->currentFrame = 1;
        this->frameRate = frameRate;
        this->shouldLoop = shouldLoop;
        this->startTime = GetTime();
    }
};