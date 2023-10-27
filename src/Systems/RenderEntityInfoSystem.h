#pragma once

#include <string>

#include <raylib.h>

#include"../ECS/ECS.h"
#include"../Components/HealthComponent.h"
#include"../Components/TransformComponent.h"
#include"../AssetStore/ColorPalette.h"
#include"../Ut/ut.h"

class RenderEntityInfoSystem : public System {
	public:
		RenderEntityInfoSystem() {
			RequireComponent<TransformComponent>();
			RequireComponent<HealthComponent>();
		}

		void Update(Rectangle& camera, std::unique_ptr<AssetStore>& assetStore) {
			for (auto& entity : GetSystemEntities()) {
				auto& transform = entity.GetComponent<TransformComponent>();
				auto& health = entity.GetComponent<HealthComponent>();
				FontData fd = assetStore->GetFontData(health.healthPercentageFont);
				
				if (health.showHealthBar)
					RenderHealthBar(camera, transform.position, transform.scale, health);
				if (health.showHealthPercentage)
					RenderHealthText(camera, transform.position, transform.scale, health.healthPercentage, fd);
			}
		}
    
    private:
        const int DEFAULT_TEXT_Y_OFFSET = 10;
		const int DEFAULT_BAR_Y_OFFSET = 3;
		const int DEFAULT_MEDIUM_THRESHOLD = 67;
		const int DEFAULT_LOW_THRESHOLD = 34;
		const int DEFAULT_BAR_W = 32;
		const int DEFAULT_BAR_H = 2;

		Color getHealthColor(int currentHealth) {

			if (currentHealth >= DEFAULT_MEDIUM_THRESHOLD) {
                return green;				
			}
			if (currentHealth < DEFAULT_MEDIUM_THRESHOLD) {
				return yellow;
			}
			if (currentHealth < DEFAULT_LOW_THRESHOLD) {
				return red;
			}

			return RAYWHITE;
		}

		void RenderHealthBar(Rectangle& camera, Vector2 position, Vector2 scale, HealthComponent& health) {//int currentHealth, int previousHealth, float lastHealthBarW) {
			int currentHealth = health.healthPercentage;
			float lastHealthBarW = health.lastHealthBarW;
			float position_y = position.y - (DEFAULT_BAR_Y_OFFSET * scale.y);
			
			Color color = getHealthColor(currentHealth);

			float fullBarW = DEFAULT_BAR_W;
			float targetBarW = (currentHealth / 100.0) * fullBarW;
			float barW = health.lastHealthBarW;

			if (lastHealthBarW != targetBarW) {
				barW = ut::LinearInterpolation(barW, targetBarW, 0.5);
			}

			health.lastHealthBarW = barW;

			Rectangle dstRect;
			dstRect.x = static_cast<int>(position.x - camera.x);
			dstRect.y = static_cast<int>(position_y - camera.y);
			dstRect.width = barW * scale.y;
			dstRect.height = (DEFAULT_BAR_H * scale.x);

			DrawRectanglePro(dstRect, Vector2{.0f, .0f}, 0.f, color);
		}

		void RenderHealthText(Rectangle& camera, Vector2 position, Vector2 scale, int currentHealth, FontData fd) {


			float position_x = position.x;
			float position_y = position.y - (DEFAULT_TEXT_Y_OFFSET * scale.y);

            std::string currentHealthStr = std::to_string(currentHealth);

            Color color = getHealthColor(currentHealth);

            DrawTextEx(
				fd.font,
				currentHealthStr.c_str(),
				Vector2{
					position_x - camera.x,
					position_y - camera.y
				},
				fd.size,
				fd.spacing,
				color
			);
		}

};