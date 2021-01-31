#pragma once

#include "framework.h"

using namespace std;

namespace RoninEngine::Runtime
{
	class ResourceManager
	{
		friend class ::RoninEngine::RoninApplication;
		friend class Scene;
		template<typename T, typename... Types>
		friend constexpr T* ::allocate_class(Types&&... _Args);
		
	protected:
		static void ResourcesInitialize();
		static void ResourcesRelease();
		static void UnloadAll(bool immediate);
		static void CheckResources();
	public:
		static void LoadImages(const char* filename);

		static void UnloadUnused();

		static void* GoMemoryCache(void* cacheToFree);

		static list<SDL_Surface*>* LoadSurfaces(const string& packName);
		static list<Runtime::Texture*>* LoadTextures(const string& packName, bool autoUnload = true);
		static SDL_Surface* GetSurface(const string& surfaceName);
		static SDL_Surface* GetSurface(const string& surfaceName, FolderKind pathOn);
		static Texture* GetTexture(const string& resourceName, FolderKind pathOn, bool autoUnload=true);
		static Texture* GetTexture(const string& resourceName, bool autoUnload=true);
		static Texture* GetTexture(const int w, const int h);
		static Texture* GetTexture(const int w, const int h, const ::SDL_PixelFormatEnum format);
		static Texture* GetTexture(const int w, const int h, const ::SDL_PixelFormatEnum format, const ::SDL_TextureAccess access);
		static SDL_Cursor* GetCursor(const string& resourceName, const Vector2i& hotspot, bool autoUnload=true);
		static SDL_Cursor* GetCursor(SDL_Surface* texture, const Vector2i& hotspot);

		static Atlas* GetAtlas(const string& atlasName);
		static Sprite* GetSprite(const string& spriteName);
		static Sprite* GetSprite(int w, int h);
		static Sprite* GetSprite(Texture* attach);

		static bool UnloadTexture(Texture* const texture);
		static void Unload(SDL_Cursor* unload);
		static void Unload(SDL_Surface* unload);
	};
}

