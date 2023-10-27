#pragma once

#include "../ECS/ECS.h"
#include "../Components/TextLabelComponent.h"
#include "../AssetStore/AssetStore.h"

class RenderTextSystem : public System {
public:
	RenderTextSystem() {
		RequireComponent<TextLabelComponent>();
	}

	void Update(Rectangle camera, std::unique_ptr<AssetStore>& assetStore) {
		for (auto entity : GetSystemEntities()) {
			auto text = entity.GetComponent<TextLabelComponent>();

			FontData fd = assetStore->GetFontData(text.assetId);

			DrawTextEx(
				fd.font,
				text.text.c_str(),
				Vector2{
					text.position.x - (text.isFixed ? 0 : camera.x),
					text.position.x - (text.isFixed ? 0 : camera.x)
				},
				fd.size,
				fd.spacing,
				text.color
			);
		}
	}
};