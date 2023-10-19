#pragma once

#include<ECS/ECS.h>
#include<Components/TextLabelComponent.h>

class RenderTextSystem : public System {
public:
	RenderTextSystem() {
		RequireComponent<TextLabelComponent>();
	}

	void Update(SDL_Renderer* renderer, SDL_Rect& camera, std::unique_ptr<AssetStore>& assetStore) {
		for (auto entity : GetSystemEntities()) {
			auto text = entity.GetComponent<TextLabelComponent>();

			SDL_Surface* surface = TTF_RenderText_Blended(
				assetStore->GetFont(text.assetId),
				text.text.c_str(),
				text.color
			);

			SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);

			SDL_FreeSurface(surface);

			int fontWidth, fontHeight;
			// Os argumentos s�o do tipo int*, mas estamos passando int&.
			// O operador * � usado para acessar o valor armazenado no endere�o apontado por um ponteiro.
			// O operador & � usado para obter o endere�o de uma vari�vel.
			// &fontWidth me d� o endere�o de fontWidth. Logo, a vari�vel int* vai ser capaz de acessar
			// o valor de fontWidth pois endere�os de mem�ria s�o compat�veis com ponteiros.
			SDL_QueryTexture(texture, NULL, NULL, &fontWidth, &fontHeight);

			SDL_Rect dstRect = {
				static_cast<int>(text.position.x - (text.isFixed ? 0 : camera.x)),
				static_cast<int>(text.position.y - (text.isFixed ? 0 : camera.y)),
				fontWidth,
				fontHeight
			};

			SDL_RenderCopy(renderer, texture, NULL, &dstRect);
		}
	}
};