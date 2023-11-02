#pragma once

#include <raylib.h>
#include <raymath.h>

#include "../ECS/ECS.h"

#include "../Components/TransformComponent.h"
#include "../Components/RigidBodyComponent.h"
#include "../Components/SpriteComponent.h"
#include "../Components/BoxColliderComponent.h"
#include "../Components/ProjectileEmitterComponent.h"
#include "../Components/ProjectileComponent.h"

#include "../Events/ProjectileRequestEvent.h"
#include "../Events/EventBus.h"

class ProjectileEmitterSystem: public System {
    public:
        ProjectileEmitterSystem() {
            RequireComponent<ProjectileEmitterComponent>();
            RequireComponent<TransformComponent>();
        }

        void SubscribeToEvents(std::unique_ptr<EventBus>& eventBus){
            eventBus->SubscribeToEvent<ProjectileRequestEvent>(this, &ProjectileEmitterSystem::OnProjectileRequest);
        };

        void Update(std::unique_ptr<Registry>& registry) {
            for (auto& entity: GetSystemEntities()){
                auto& projectileEmitter = entity.GetComponent<ProjectileEmitterComponent>();

                // Projectiles with repeatFrequency < 0 should be handled by OnProjectileRequest.
                if(projectileEmitter.repeatFrequency < 0){
                    continue;
                }

                const auto& transform = entity.GetComponent<TransformComponent>();

                if(GetTime() - projectileEmitter.lastEmissionTime > projectileEmitter.repeatFrequency){
                    Vector2 projectilePos = transform.position;
                    if (entity.HasComponent<SpriteComponent>()){
                        const auto sprite = entity.GetComponent<SpriteComponent>();
                        projectilePos.x += (transform.scale.x * sprite.width / 2);
                        projectilePos.y += (transform.scale.y * sprite.height / 2);                        
                    }

                    Entity projectile = registry->CreateEntity();

                    if (entity.HasTag(Tag::PLAYER)) {
                        projectile.AddTag(Tag::PLAYER_PROJECTILE);
                    }
                    else {
                        projectile.AddTag(Tag::ENEMY_PROJECTILE);
                    }

                    Vector2 projVel;
                    if (projectileEmitter.targetEntityId) {

                        Vector2 targetPos = registry->GetComponent<TransformComponent>(projectileEmitter.targetEntityId).position;
                        Vector2 pos = projectilePos;
                        Vector2 direction = Vector2Normalize(Vector2Subtract(targetPos, pos));
                        projVel = Vector2Scale(direction, projectileEmitter.projectileVelocity.x);
                    }
                    else {
                        projVel = projectileEmitter.projectileVelocity;
                    }

                    projectile.AddComponent<TransformComponent>(projectilePos, transform.scale);
                    projectile.AddComponent<RigidBodyComponent>(projVel);
                    projectile.AddComponent<BoxColliderComponent>(4, 4);
                    projectile.AddComponent<SpriteComponent>("bullet-image", LAYER_2, 4, 4);
                    projectile.AddComponent<ProjectileComponent>(projectileEmitter.isFriendly, projectileEmitter.hitPercentDamage, projectileEmitter.projectileDuration);
                    
                    projectileEmitter.lastEmissionTime = GetTime();
                }
            }
        }


        void OnProjectileRequest(ProjectileRequestEvent& event){
            auto& entity = event.entity;
            auto& projectileEmitter = entity.GetComponent<ProjectileEmitterComponent>();

            if(GetTime() - projectileEmitter.lastEmissionTime < projectileEmitter.cooldown)
                return;

            auto registry = entity.GetRegistry();
            const auto& transform = entity.GetComponent<TransformComponent>();
            const auto& rigidBody = entity.GetComponent<RigidBodyComponent>();
            const auto& sprite = entity.GetComponent<SpriteComponent>();

            Vector2 projectilePos = transform.position;
            projectilePos.x += (transform.scale.x * sprite.width / 2);
            projectilePos.y += (transform.scale.y * sprite.height / 2);

            float projectileSpeed = projectileEmitter.projectileVelocity.x;
            Vector2 projectileVelocity = 
                Vector2Normalize(rigidBody.velocity).x != 0 
                ? Vector2{Vector2Normalize(rigidBody.velocity).x * projectileSpeed, .0f} 
                : Vector2{.0f, Vector2Normalize(rigidBody.velocity).y * projectileSpeed};

            Entity projectile = registry->CreateEntity();

            if (entity.HasTag(Tag::PLAYER)) {
                projectile.AddTag(Tag::PLAYER_PROJECTILE);
            }
            else {
                projectile.AddTag(Tag::ENEMY_PROJECTILE);
            }

            projectile.AddComponent<TransformComponent>(projectilePos, transform.scale);
            projectile.AddComponent<RigidBodyComponent>(projectileVelocity);
            projectile.AddComponent<BoxColliderComponent>(4, 4);
            projectile.AddComponent<SpriteComponent>("bullet-image", LAYER_2, 4, 4);
            projectile.AddComponent<ProjectileComponent>(projectileEmitter.isFriendly, projectileEmitter.hitPercentDamage, projectileEmitter.projectileDuration);
            projectileEmitter.lastEmissionTime = GetTime();
            projectileEmitter.emissionRequestedByEvent = false;
        }
};