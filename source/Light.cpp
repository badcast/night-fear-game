#include "pch.h"
#include "Light.h"
#include "inputSystem.h"

namespace RoninEngine::Runtime
{
	Light::Light() : Light(typeid(Light).name()) {

	}
	Light::Light(const string& name) : Component(name) {
		fieldFogTexture = NULL;
	}

	Light::~Light() {
		if (fieldFogTexture)
			ResourceManager::UnloadTexture(fieldFogTexture);
	}

	void Light::GetLightSource(Render_info* renderer_info) {
		auto display = RoninApplication::display();
		SDL_Texture* _target = NULL;
		int cRes;

		if (!fieldFogTexture)
		{
			Color c;
			fieldFogTexture = Texture::Create_TargetTexture(display.w, display.h, SDL_PIXELFORMAT_RGBA8888);
			fieldFogTexture->blendMode(SDL_BlendMode::SDL_BLENDMODE_BLEND); 

			c = fieldFogTexture->color();
			c.r = 25;
			c.g = 25;
			c.b = 25;
			c.a = 25;

			fieldFogTexture->color(c);

		}
		_target = SDL_GetRenderTarget(renderer_info->renderer);
		SDL_SetRenderTarget(renderer_info->renderer, fieldFogTexture->native());
		SDL_SetRenderDrawColor(renderer_info->renderer, 0, 0, 0, 255);
		SDL_RenderClear(renderer_info->renderer);

		//Copy empty background
		cRes = SDL_RenderCopy(renderer_info->renderer, fieldFogTexture->native(), NULL, NULL);
		//belting light a top background
		this->GetLight(renderer_info->renderer);
		cRes = SDL_SetRenderTarget(renderer_info->renderer, _target);
		cRes = SDL_RenderCopy(renderer_info->renderer, fieldFogTexture->native(), NULL, NULL); // draw 
		renderer_info->dst.y = renderer_info->src.x = 0;
		renderer_info->dst.x = renderer_info->src.y = 0;
		renderer_info->dst.w = renderer_info->src.w = display.w;
		renderer_info->dst.h = renderer_info->src.h = display.h;

		//renderer_info->texture = texture;
	}
}
