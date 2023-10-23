#pragma once

#include <iostream>

#include <Raylib.h>

#include "../ECS/ECS.h"
#include "../Components/TransformComponent.h"
#include "../Components/SpriteComponent.h"
#include "../AssetStore/AssetStore.h"



class RenderSystem: public System {
    public:
        RenderSystem() {
            RequireComponent<TransformComponent>();
            RequireComponent<SpriteComponent>();
        }

        void Update(Rectangle camera, std::unique_ptr<AssetStore>& assetStore) {
            // Sort entities by z-index. Possivelmente lento.
            // Talvez zeja melhor fazê-lo no momento da criação da entidade.
            // Outra opção é ter um vetor diferente para cada layer.
            // Ou talvez um map de referências as entidades, onde a chave é o índice.
            // Usando o observer pattern, enviar um sinal cada vez que um spriteComponent é adicionado a uma entidade.
            // Esse sinal ativa um método que insere a referência da entidade sob a chave relevante.

            // Usando Insertion Sort. Esse algoritmo é mais rápido quando os dados
            // estão quase ordenados. Isto é, a primeira execução será lenta,
            // mas as demais deverão ser mais rápidas.
            SortRenderEntities();

            for (auto entity: GetSystemEntities()){
                const auto& transform = entity.GetComponent<TransformComponent>();
                const auto& sprite = entity.GetComponent<SpriteComponent>();

                if (ShouldCullEntity(camera, transform, sprite))
                    continue;

                Rectangle srcRect = sprite.srcRect;

                Vector2 origem = Vector2{transform.position.x - (sprite.isFixed ? 0 : camera.x), transform.position.y - (sprite.isFixed ? 0 : camera.y)};

                Rectangle dstRect = {
                    origem.x,
                    origem.y,
                    sprite.width * transform.scale.x,
                    sprite.height * transform.scale.y
                };

                DrawTexturePro(
                    assetStore->GetTexture(sprite.assetId),
                    srcRect,
                    dstRect,
                    origem,
                    transform.rotation,
                    WHITE
                );
            }
        }

    private:
        void SortRenderEntities() {
            auto sortedEntities = GetSystemEntities();

            for (std::size_t j = 1; j < sortedEntities.size(); j++) {
                Entity keyEntity = sortedEntities[j];
                unsigned int keyEntityzIndex = keyEntity.GetComponent<SpriteComponent>().zIndex;
                int i = j - 1;

                while (i >= 0 && sortedEntities[i].GetComponent<SpriteComponent>().zIndex > keyEntityzIndex) {
                    sortedEntities[i + 1] = sortedEntities[i];
                    i--;
                }
                sortedEntities[i + 1] = keyEntity;
            }

            SetSystemEntities(sortedEntities);
        }

        bool ShouldCullEntity(Rectangle camera, TransformComponent transform, SpriteComponent sprite) {
            if (sprite.isFixed)
                return false;

            Vector2 position = transform.position;
            float scaleX = transform.scale.x;
            float scaleY = transform.scale.y;
            int spriteW = sprite.width;
            int spriteH = sprite.height;

            if (position.x + spriteW * scaleX < camera.x||
                position.x > camera.x + camera.width) {
                return true;
            }

            if (position.y + spriteH * scaleY < camera.y ||
                position.y > camera.y + camera.height + spriteH) {
                return true;
            }

            return false;
        }
};