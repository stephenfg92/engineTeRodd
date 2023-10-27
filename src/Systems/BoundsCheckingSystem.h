#pragma once

#include "../Game/Game.h"

#include "../ECS/ECS.h"
#include "../ECS/Tag.h"

#include "../Components/TransformComponent.h"


class BoundsCheckingSystem: public System {
	public:
		BoundsCheckingSystem() {
			RequireComponent<TransformComponent>();
		}

		void Update(int maxW, int maxH) {

			for (auto entity: GetSystemEntities()) {
				auto& transform = entity.GetComponent<TransformComponent>();
				auto& position = transform.position;
				bool isWithinBounds = (position.x > maxW || position.x < 0 || position.y > maxH || position.y < 0);
				
				if (!isWithinBounds)
					continue;

				if (entity.HasTag(Tag::PLAYER)) {
					if (position.x < 0)
						position.x = 0;
					if (position.y < 0)
						position.y = 0;
					if (position.x > maxW)
						position.x = maxW;
					if (position.y > maxH)
						position.y = maxH;
					continue;
				}

				entity.Destroy();
			}
		}
};