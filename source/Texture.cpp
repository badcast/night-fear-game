#include "pch.h"
#include "Texture.h"

namespace RoninEngine::Runtime {
	Texture::Texture() : Texture(32, 32) {
	}

	Texture::Texture(SDL_Surface* surf) {
		if (!surf || !(m_native = SDL_CreateTextureFromSurface(RoninEngine::RoninApplication::GetRenderer(), surf)))
			RoninApplication::back_fail();
		flag = 0x1;
	}

	Texture::Texture(SDL_Texture* native, bool grub) {
		if (!native)
			RoninApplication::back_fail();

		flag = grub ? 0x1 : 0;
		m_native = native;

	}
	Texture::Texture(const int w, const int h) : Texture(w, h, SDL_PixelFormatEnum::SDL_PIXELFORMAT_RGBA8888) {
	}
	Texture::Texture(const int w, const int h, const SDL_PixelFormatEnum format) : Texture(w, h, format, SDL_TextureAccess::SDL_TEXTUREACCESS_STATIC) {
	}

	Texture::Texture(const int w, const int h, const SDL_PixelFormatEnum format, const SDL_TextureAccess access) {
		m_native = SDL_CreateTexture(RoninEngine::RoninApplication::GetRenderer(), format, access, w, h);
		if (!m_native)
			RoninApplication::back_fail();

		flag = 0x1;
	}

	Texture::~Texture() {
       if (raw_namePtr)
           SDL_free(raw_namePtr);

		if (flag == 0x1)
           if (m_native)
           {
               SDL_DestroyTexture(m_native);
               m_native = nullptr;
           }

	}

	const bool Texture::valid() {
		return !(!m_native || width() <= 0 || height() <= 0);
	}

	const int Texture::width() {
		int w;
		if (SDL_QueryTexture(m_native, NULL, NULL, &w, NULL))
			RoninApplication::back_fail();
		return w;
	}
	const int Texture::height() {
		int h;
		if (SDL_QueryTexture(m_native, NULL, NULL, NULL, &h))
			RoninApplication::back_fail();
		return h;
	}

	const SDL_PixelFormatEnum Texture::format() {
		uint32_t f;
		if (SDL_QueryTexture(m_native, &f, NULL, NULL, NULL))
			RoninApplication::back_fail();
		return static_cast<SDL_PixelFormatEnum>(f);
	}

	const SDL_BlendMode Texture::blendMode() {
        SDL_BlendMode bmode;

		if (SDL_GetTextureBlendMode(m_native, &bmode))
			RoninApplication::back_fail();

		return bmode;
	}
	const void Texture::blendMode(const SDL_BlendMode blendMode) {
		if (SDL_SetTextureBlendMode(m_native, blendMode))
			RoninApplication::fail("Error unsupported operation");
	}
	const SDL_ScaleMode Texture::scaleMode() {
		::SDL_ScaleMode smode;

		if (SDL_GetTextureScaleMode(m_native, &smode))
			RoninApplication::back_fail();

		return smode;
	}
	const void Texture::scaleMode(const SDL_ScaleMode scaleMode) {
		if (SDL_SetTextureScaleMode(m_native, scaleMode))
			RoninApplication::back_fail();
	}

	const SDL_TextureAccess Texture::access() {
		int ac;
		if (SDL_QueryTexture(m_native, NULL, &ac, NULL, NULL))
			RoninApplication::back_fail();
		return static_cast<SDL_TextureAccess>(ac);
	}

	const Color Texture::color() {
		Color c;
		if (!(!SDL_GetTextureColorMod(m_native, &c.r, &c.g, &c.b) && !SDL_GetTextureAlphaMod(m_native, &c.a)))
			RoninApplication::back_fail();

		return c;
	}
	const void Texture::color(const Color value) {
		if (!(!SDL_SetTextureColorMod(m_native, value.r, value.g, value.b) && !SDL_SetTextureAlphaMod(m_native, value.a)))
			RoninApplication::back_fail();
	}

	void Texture::lockTexture(const SDL_Rect* rect, void** pixels, int* pitch) {
      SDL_LockTexture(m_native, rect, pixels, pitch);
	}
	void Texture::unlockTexture() {
      SDL_UnlockTexture(m_native);
	}

	SDL_Texture* Texture::native() {
		return m_native;
	}

	const SDL_Texture* Texture::cnative() {
		return m_native;
	}

	const SDL_Rect Texture::getRect() {
		return { 0, 0, width(), height() };
	}

	const Texture* Texture::clone() {
		return clone(RoninApplication::GetRenderer());
	}
	const Texture* Texture::clone(SDL_Renderer* renderer) {
		SDL_Texture* __t = SDL_GetRenderTarget(renderer);
		Texture* _n;
		allocate_variable(_n, width(), height(), format(), access());
		_n->blendMode(blendMode());
		_n->scaleMode(scaleMode());
		_n->color(color());
		SDL_CreateTextureFromSurface(renderer, NULL);
		SDL_SetRenderTarget(renderer, _n->m_native);
		SDL_RenderCopy(renderer, m_native, NULL, NULL);
		SDL_SetRenderTarget(renderer, __t);
		return _n;
	}

	const string Texture::name() {
		return string(raw_namePtr == NULL ? "Unknown" : raw_namePtr);
	}


	Texture* Texture::Create_TargetTexture(const int w, const int h, const ::SDL_PixelFormatEnum format) {
		return ResourceManager::GetTexture(w, h, format, SDL_TextureAccess::SDL_TEXTUREACCESS_TARGET);
	}
	Texture* Texture::Create_Texture(const int w, const int h, const ::SDL_PixelFormatEnum format, const ::SDL_TextureAccess access) {
		Texture* tex = ResourceManager::GetTexture(w, h, format, access);
		return tex;
	}
}
