#pragma once

#include <raylib.h>

#include "../ECS/ECS.h"
#include "../Components/CameraFollowComponent.h"
#include "../Components/TransformComponent.h"


class CameraMovementSystem: public System {
    public:
        CameraMovementSystem(){
            RequireComponent<CameraFollowComponent>();
            RequireComponent<TransformComponent>();
        }

        void Update(Rectangle& camera){
            for (auto entity: GetSystemEntities()){
                auto& transform = entity.GetComponent<TransformComponent>();

                // Isso pode ser simplificado usando glm::clamp, só não sei como. O mais perto que cheguei foi:
                /*camera.x = glm::clamp<int>(transform.position.x - (Game::windowWidth / 2), 0, Game::mapWidth - (Game::windowWidth / 2));
                camera.y = glm::clamp<int>(transform.position.y - (Game::windowHeight / 2), 0, Game::mapHeight - (Game::windowHeight / 2));*/

                if (transform.position.x + (camera.width / 2) < Game::mapWidth) {
                    camera.x = transform.position.x - (Game::larguraTela / 2);
                }

                if (transform.position.y + (camera.height / 2) < Game::mapHeight) {
                    camera.y = transform.position.y - (Game::alturaTela / 2);
                }

                camera.x = camera.x < 0 ? 0 : camera.x;
                camera.y = camera.y < 0 ? 0 : camera.y;
                camera.x = camera.x > camera.width ? camera.width : camera.x;
                camera.y = camera.y > camera.height ? camera.height : camera.y;

            }
        }
};