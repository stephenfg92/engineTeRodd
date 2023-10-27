#pragma once

#include <string>
#include <raylib.h>

struct TextLabelComponent {
	std::string assetId;
	std::string text;
	Vector2 position;
	Color color;
	bool isFixed;

	TextLabelComponent(
		std::string assetId = "",
		std::string text = "",
		Vector2 position = Vector2{.0f, .0f},
		const Color& color = { 255, 255, 255, 1 },
		bool isFixed = true
	) {
		this->assetId = assetId;
		this->text = text;
		this->position = position;
		this->color = color;
		this->isFixed = isFixed;
	};
};