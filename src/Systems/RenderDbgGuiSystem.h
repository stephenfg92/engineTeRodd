#pragma once

#include <raylib.h>

#include "../imgui/imgui.h"
#include "../imgui/imgui_impl_raylib.h"

#include "../ECS/ECS.h"

#include "../DbgWindows/SpawnEntityWindow.h"
#include "../DbgWindows/MousePosWindow.h"

class RenderDbgGuiSystem : public System {
	private:
		SpawnEntityWindow spawnEntityWindow;
		MousePosWindow mousePosWindow;

	public:
		RenderDbgGuiSystem() = default;

		void Update(const std::unique_ptr<Registry>& registry, const std::unique_ptr<AssetStore>& assetStore, Rectangle camera) {
        	ImGui_ImplRaylib_NewFrame();
        	ImGui::NewFrame();

			//ImGui::ShowDemoWindow();
			spawnEntityWindow.Show(registry, assetStore);
			mousePosWindow.Show(camera);
			

			ImGui::Render();
			ImGui_ImplRaylib_RenderDrawData(ImGui::GetDrawData());
		}
};