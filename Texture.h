#pragma once

#include "framework.h"

namespace RoninEngine::Runtime
{
	class Camera;
	class ResourceManager;

	class Texture
	{
		friend class Camera;
		friend class ResourceManager;

	private:
		int flag;
		::SDL_Texture* m_native;
		void destroyNative();
		char* raw_namePtr;
	public:

		Texture();
		explicit Texture(::SDL_Surface* surface);
		explicit Texture(::SDL_Texture* native, bool grub = true);
		explicit Texture(const int w, const int h);
		explicit Texture(const int w, const int h, const ::SDL_PixelFormatEnum format);
		explicit Texture(const int w, const int h, const ::SDL_PixelFormatEnum format, ::SDL_TextureAccess access);

		~Texture();

		const bool valid();

		const int width();
		const int height();

		const SDL_PixelFormatEnum format();

		const SDL_BlendMode blendMode();
		const void blendMode(const SDL_BlendMode blendMode);
		const SDL_ScaleMode scaleMode();
		const void scaleMode(const SDL_ScaleMode scaleMode);

		const SDL_TextureAccess access();

		const Color color();
		const void color(const Color value);

		void lockTexture(const SDL_Rect* rect, void** pixels, int* pitch);
		void unlockTexture();

		void destroy();

		SDL_Texture* native();
		const SDL_Texture* cnative();

		const SDL_Rect getRect();

		//Create identity texture
		const Texture* clone();
		//Create identity texture
		const Texture* clone(SDL_Renderer* renderer);

		const string name();

		static Texture* Create_TargetTexture(const int w, const int h, const ::SDL_PixelFormatEnum format);
		static Texture* Create_Texture(const int w, const int h, const ::SDL_PixelFormatEnum format, const ::SDL_TextureAccess access);
	};
}

