#pragma once

#include <vector>

#include <raymath.h>

#include "../imgui/imgui.h"

#include "../DbgWindows/Window.h"
#include "../ECS/ECS.h"
#include "../ut/ut.h"

#include "../Components/TransformComponent.h"
#include "../Components/SpriteComponent.h"
#include "../Components/AnimationComponent.h"
#include "../Components/RigidBodyComponent.h"
#include "../Components/BoxColliderComponent.h"
#include "../Components/HealthComponent.h"
#include "../Components/ProjectileEmitterComponent.h"



class SpawnEntityWindow : public Window {
	public:
		SpawnEntityWindow() = default;

		void Show(const std::unique_ptr<Registry>& registry, const std::unique_ptr<AssetStore>& assetStore){
			if (ImGui::Begin("Spawn Enemies")) {

				// Position
				ImGui::Text("Position");
				static float xPos = 0;
				static float yPos = 0;
				ImGui::InputFloat("x position", &xPos);
				ImGui::InputFloat("y position", &yPos);

				// Velocity
				ImGui::Text("Velocity");
				static float xVel = 0;
				static float yVel = 0;
				ImGui::InputFloat("x velocity", &xVel);
				ImGui::InputFloat("y velocity", &yVel);

				// Angle
				ImGui::Text("Projectile");
				static ImGuiSliderFlags sliderFlags = ImGuiSliderFlags_None;
				static float projAngle = 0.5f;
				static float projMag = 0.0f;
				const float min = 0.f, max = 999.9f;
				ImGui::DragFloat("Angle", &projAngle, 0.1f, 0.0f, 360.0f, "%.1f degrees", sliderFlags);
				ImGui::DragScalar("Magnitude", ImGuiDataType_Float, &projMag, 0.1f, &min, &max, "%.1f");
				static int projFreq = 1;
				static int projDuration = 1;
				ImGui::InputInt("Repeat Frequency (seconds)", &projFreq);
				ImGui::InputInt("Duration (seconds)", &projDuration);
				static int projDmg;
				ImGui::DragInt("Damage", &projDmg, 1, 0, 100);


				// Sprite
				ImGui::Text("Sprite (list won't update during runtime)");
				static ImGuiComboFlags spriteFlags = 0;
				//if (firstTimeRendering) // Impede do get ser feito todo loop. N�o atualiza caso assets sejam adicionados durante o runtime.
				const std::vector<std::string> items = assetStore->GetTextureIds();
				static int item_current_idx = 0;
				const char* combo_preview_value = items[item_current_idx].c_str();

				if (ImGui::BeginCombo("spriteId", combo_preview_value, spriteFlags))
				{
					for (size_t i = 0; i < items.size(); i++) {
						const bool is_selected = (static_cast<size_t>(item_current_idx) == i);
						if (ImGui::Selectable(items[i].c_str(), is_selected)) {
							item_current_idx = i;

							if (is_selected)
								ImGui::SetItemDefaultFocus();
						}
					}
					ImGui::EndCombo();
				}

				if (ImGui::Button("Spawn Enemy")) {
					SpawnEnemyArgs args = {
						xPos, yPos,
						xVel, yVel,
						projAngle, projMag,
						projFreq, projDuration, projDmg,
						items[item_current_idx]
					};
					SpawnEnemy(registry, args);
				}

			}
			ImGui::End();
			//firstTimeRendering = false;
		}

	private:
		bool firstTimeRendering = true;

		struct SpawnEnemyArgs {
			float xPos, yPos;
			float xVel, yVel;
			float projAngle, projMag;
			int projFreq, projDuration, projDamage;
			std::string assetId;
		};

		void SpawnEnemy(const std::unique_ptr<Registry>& registry, SpawnEnemyArgs args) {

			Entity enemy = registry->CreateEntity();
			enemy.AddTag(Tag::ENEMY);
			enemy.AddComponent<TransformComponent>(Vector2{args.xPos, args.yPos}, Vector2{3.0f, 3.0f}, 0);
			enemy.AddComponent<RigidBodyComponent>(Vector2{args.xVel, args.yVel});
			enemy.AddComponent<SpriteComponent>(args.assetId, LAYER_1, 32, 32);
			enemy.AddComponent<BoxColliderComponent>(32, 32);
			enemy.AddComponent<HealthComponent>(100, true, "charriot-font");

			float rads = ut::DegToRad(args.projAngle);
			Vector2 projVel = Vector2Scale(ut::RadToVec(rads), args.projMag);
			enemy.AddComponent<ProjectileEmitterComponent>(projVel, args.projFreq * 1000, args.projDuration * 1000, args.projDamage);
		}
};