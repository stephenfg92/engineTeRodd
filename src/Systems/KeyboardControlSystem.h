#pragma once

#include <raylib.h>

#include "../Events/EventBus.h"
#include "../Events/KeyPressedEvent.h"
#include "../Events/ProjectileRequestEvent.h"

#include "../Components/KeyboardControlComponent.h"
#include "../Components/SpriteComponent.h"
#include "../Components/RigidBodyComponent.h"
#include "../Components/ProjectileEmitterComponent.h"


class KeyboardControlSystem: public System {
    public:
        KeyboardControlSystem() {
            RequireComponent<KeyboardControlComponent>();
            RequireComponent<SpriteComponent>();
            RequireComponent<RigidBodyComponent>();
        }

        void Update(std::unique_ptr<EventBus>& eventBus) {
            for (auto entity: GetSystemEntities()) {
                if(entity.HasComponent<ProjectileEmitterComponent>()){
                    auto& projectileEmitter = entity.GetComponent<ProjectileEmitterComponent>();
                    if (projectileEmitter.emissionRequestedByEvent){
                        eventBus->EmitEvent<ProjectileRequestEvent>(entity);
                    }
                }


                const auto& keyboardControl = entity.GetComponent<KeyboardControlComponent>();
                auto& sprite = entity.GetComponent<SpriteComponent>();
                auto& rigidBody = entity.GetComponent<RigidBodyComponent>();

                if (IsKeyDown(KEY_UP)){
                    rigidBody.velocity = keyboardControl.upVelocity;
                    sprite.srcRect.y = sprite.height * 0;
                }

                if (IsKeyDown(KEY_RIGHT)){
                    rigidBody.velocity = keyboardControl.rightVelocity;
                    sprite.srcRect.y = sprite.height * 1;
                }

                if (IsKeyDown(KEY_DOWN)){
                    rigidBody.velocity = keyboardControl.downVelocity;
                    sprite.srcRect.y = sprite.height * 2;
                }

                if (IsKeyDown(KEY_LEFT)){
                    rigidBody.velocity = keyboardControl.leftVelocity;
                    sprite.srcRect.y = sprite.height * 3;
                }

                if (IsKeyDown(KEY_SPACE)){
                    if(entity.HasComponent<ProjectileEmitterComponent>()) {
                        auto& projectileEmitter = entity.GetComponent<ProjectileEmitterComponent>();
                        projectileEmitter.emissionRequestedByEvent = true;
                    } 
                }
            }
        }
};