#pragma once

#include <raylib.h>

#include "../ECS/ECS.h"
#include "../Components/SpriteComponent.h"
#include "../Components/AnimationComponent.h"

class AnimationSystem: public System {
    public:
        AnimationSystem() {
            RequireComponent<AnimationComponent>();
            RequireComponent<SpriteComponent>();
        }

        void Update() {
            for (auto entity: GetSystemEntities()){
                auto& animation = entity.GetComponent<AnimationComponent>();
                auto& sprite = entity.GetComponent<SpriteComponent>();

                animation.currentFrame = static_cast<int>((GetTime() - animation.startTime) * animation.frameRate) % animation.numFrames;

                sprite.srcRect.x = animation.currentFrame * sprite.width;
            }
        }
};