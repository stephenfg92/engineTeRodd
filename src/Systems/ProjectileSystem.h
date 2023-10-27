#pragma once

#include <raylib.h>

#include "../ECS/ECS.h"
#include "../Components/ProjectileComponent.h"

class ProjectileSystem: public System {
    public:
        ProjectileSystem() {
            RequireComponent<ProjectileComponent>();
        }

        void Update() {
            for (auto entity: GetSystemEntities()) {
                auto& projectile = entity.GetComponent<ProjectileComponent>();

                if (GetTime() - projectile.startTime > projectile.duration) {
                    entity.Destroy();
                }
            }
        }
};