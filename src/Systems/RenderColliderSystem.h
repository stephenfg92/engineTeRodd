#pragma once

#include <raylib.h>

#include "../Components/BoxColliderComponent.h"

class RenderColliderSystem : public System {
    public:
        RenderColliderSystem(){
            RequireComponent<BoxColliderComponent>();
            RequireComponent<TransformComponent>();
        }

    void Update(Rectangle camera) {
        for (auto entity: GetSystemEntities()){
                auto& collider = entity.GetComponent<BoxColliderComponent>();
                auto& transform =entity.GetComponent<TransformComponent>();

                DrawRectangleLines(
                    static_cast<int>((transform.position.x + (collider.offset.x * transform.scale.x)) - camera.x), 
                    static_cast<int>((transform.position.y + (collider.offset.y * transform.scale.y)) - camera.y), 
                    static_cast<int>(collider.width * transform.scale.x), 
                    static_cast<int>(collider.height * transform.scale.y), 
                    RAYWHITE
                );
        }
    }
};