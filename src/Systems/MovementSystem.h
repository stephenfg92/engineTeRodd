#pragma once

#include <raymath.h>

#include "../ECS/ECS.h"

#include "../Components/TransformComponent.h"
#include "../Components/RigidBodyComponent.h"
#include "../Components/SpriteComponent.h"

#include "../Events/EventBus.h"
#include "../Events/CollisionEvent.h"

class MovementSystem: public System {
    public:
        MovementSystem() {
            RequireComponent<TransformComponent>();
            RequireComponent<RigidBodyComponent>();
        }

        void SubscribeToEvents(const std::unique_ptr<EventBus>& eventBus) {
            eventBus->SubscribeToEvent<CollisionEvent>(this, &MovementSystem::OnCollision);
        }

        void Update(double deltaTime) {
            for (auto entity: GetSystemEntities()){
                TransformComponent& transform = entity.GetComponent<TransformComponent>();
                const RigidBodyComponent rigidbody = entity.GetComponent<RigidBodyComponent>();

                transform.position.x += ((rigidbody.velocity.x) * deltaTime);
                transform.position.y += ((rigidbody.velocity.y) * deltaTime);
            }
        }

    private:
        void OnCollision(CollisionEvent& event) {
            if (event.a.HasTag(Tag::ENEMY) && event.b.HasTag(Tag::OBSTACLE))
                OnEnemyCollidesWithObstacle(event.a);

            if (event.b.HasTag(Tag::ENEMY) && event.a.HasTag(Tag::OBSTACLE))
                OnEnemyCollidesWithObstacle(event.b);

        }

        void OnEnemyCollidesWithObstacle(Entity entity) {
            if (entity.HasComponent<SpriteComponent>()) {
                SpriteComponent& sprite = entity.GetComponent<SpriteComponent>();
                //sprite.flip = (sprite.flip == SDL_FLIP_NONE) ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;
            }
                

            if (entity.HasComponent<RigidBodyComponent>()){
                Vector2 velocity =  entity.GetComponent<RigidBodyComponent>().velocity;
                velocity = Vector2Scale(velocity, -1);
                entity.GetComponent<RigidBodyComponent>().velocity = velocity;
            }
        }
};