#pragma once

#include <vector>

#include <raylib.h>

#include "../DbgWindows/Window.h"
#include "../ECS/ECS.h"
#include "../ut/ut.h"
#include "../imgui/imgui.h"


class MousePosWindow : public Window {
public:
	MousePosWindow() = default;

	void Show(Rectangle camera){
        ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoNav;
        ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_Always, ImVec2(0, 0));
        ImGui::SetNextWindowBgAlpha(0.9f);
        if (ImGui::Begin("Map coordinates", NULL, windowFlags)) {
            ImGui::Text(
                "Map coordinates (x=%.1f, y=%.1f)",
                ImGui::GetIO().MousePos.x + camera.x,
                ImGui::GetIO().MousePos.y + camera.y
            );
        }
        ImGui::End();
	}
};