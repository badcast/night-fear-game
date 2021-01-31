#include "pch.h"
#include "Spotlight.h"

namespace RoninEngine::Runtime
{
	static int usedCount = 0;
	static Texture* spotlightMask = NULL;

	Spotlight::Spotlight() : Spotlight(typeid(Spotlight).name()) {

	}
	Spotlight::Spotlight(const string& name) : Light(name) {
		range = 1.f;
		intensity = 1.f;
		if (!spotlightMask)
		{
			spotlightMask = ResourceManager::GetTexture("spotlight", FolderKind::TEXTURES);
			spotlightMask->blendMode(SDL_BlendMode::SDL_BLENDMODE_BLEND);
		}
		++usedCount;
	}

	Spotlight::~Spotlight() 
	{
		if (!(--usedCount) && spotlightMask) {
			ResourceManager::UnloadTexture(spotlightMask);
			spotlightMask = NULL;
		}
	}
	
	void Spotlight::GetLight(SDL_Renderer* renderer) {
		SDL_RenderCopyEx(renderer, spotlightMask->native(), NULL, NULL, transform()->angle(), NULL, SDL_RendererFlip::SDL_FLIP_NONE);
	}
	
}
