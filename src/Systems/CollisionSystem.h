#pragma once

#include "../ECS/ECS.h"
#include "../Components/BoxColliderComponent.h"
#include "../Components/TransformComponent.h"
#include "../Events/EventBus.h"
#include "../Events/CollisionEvent.h"

#include <iostream>

class CollisionSystem: public System {
    public:
        CollisionSystem() {
            RequireComponent<BoxColliderComponent>();
            RequireComponent<TransformComponent>();  
        }

        bool CheckAABBCollision(
            double aX, double aY, double aW, double aH,
            double bX, double bY, double bW, double bH
        ){
            return (
                aX < bX + bW &&
                aX + aW > bX &&
                aY < bY + bH &&
                aY + aH > bY
            );
        }

        void Update(std::unique_ptr<EventBus>& eventBus) {
            auto entities = GetSystemEntities();

            // .begin e end são iterator pointers, por isso o operador de
            // desreferenciamento se faz necessário.
            for (auto i = entities.begin(); i != entities.end(); i++){
                Entity a = *i;
                auto& aTransform = a.GetComponent<TransformComponent>();
                auto& aCollider  = a.GetComponent<BoxColliderComponent>();

                // j = i + i. Se j = i, estaríamos testando a colisão de uma entidade
                // com ela mesma.
                for (auto j = i + 1; j!= entities.end(); j++) {
                    Entity b = *j;
                    auto& bTransform = b.GetComponent<TransformComponent>();
                    auto& bCollider  = b.GetComponent<BoxColliderComponent>();

                    bool isColliding = CheckAABBCollision(
                        aTransform.position.x + (aCollider.offset.x * aTransform.scale.x),
                        aTransform.position.y + (aCollider.offset.y * aTransform.scale.y),
                        aCollider.width * aTransform.scale.x,
                        aCollider.height * aTransform.scale.y,
                        bTransform.position.x + (bCollider.offset.x * bTransform.scale.x),
                        bTransform.position.y + (bCollider.offset.y * bTransform.scale.y),
                        bCollider.width * bTransform.scale.x,
                        bCollider.height * bTransform.scale.y
                    );

                    if (isColliding){
                        eventBus->EmitEvent<CollisionEvent>(a,b);
                    }

                }
            }
        }
};