#pragma once

#include "framework.h"

using namespace std;

namespace RoninEngine::Runtime {

constexpr int GCInvalidID = 0xffffffff;

class GC {
    friend class ::RoninEngine::RoninApplication;
    friend class Scene;

   protected:
    static void gc_init();
    static void gc_free();
    static void gc_free_source();
    static void gc_collect();

    static void UnloadAll(bool immediate);
    static void CheckResources();

    static void suspend_gc();
    static void continue_gc();

   public:
    [[deprecate]] static void LoadImages(const char *filename);
    [[deprecate]] static void UnloadUnused();
    [[deprecate]] static void *GoMemoryCache(void *cacheToFree);

    [[deprecate]] static list<SDL_Surface *> *LoadSurfaces(const string &packName);
    [[deprecate]] static list<Runtime::Texture *> *LoadTextures(const string &packName, bool autoUnload = true);
    [[deprecate]] static SDL_Surface *GetSurface(const string &surfaceName);
    [[deprecate]] static SDL_Surface *GetSurface(const string &surfaceName, FolderKind pathOn);
    [[deprecate]] static Texture *GetTexture(const string &resourceName, FolderKind pathOn, bool autoUnload = true);
    [[deprecate]] static Texture *GetTexture(const string &resourceName, bool autoUnload = true);
    [[deprecate]] static Texture *GetTexture(const int w, const int h);
    [[deprecate]] static Texture *GetTexture(const int w, const int h, const ::SDL_PixelFormatEnum format);
    [[deprecate]] static Texture *GetTexture(const int w, const int h, const ::SDL_PixelFormatEnum format,
                                             const ::SDL_TextureAccess access);
    [[deprecate]] static SDL_Cursor *GetCursor(const string &resourceName, const point_t &hotspot, bool autoUnload = true);
    [[deprecate]] static SDL_Cursor *GetCursor(SDL_Surface *texture, const point_t &hotspot);

    [[deprecate]] static Atlas *GetAtlas(const string &atlasName);

    static void *gc_malloc(std::size_t size);

    template <typename T, typename... Args>
    static T *gc_push_lvalue(T *&lvalue_pointer, Args &&...arguments) {
        return lvalue_pointer = gc_push<T>(std::forward<Args&&>(arguments)...);
    }

    template <typename T, typename... Args>
    static T *gc_push(Args ...arguments);

    template <typename T>
    static bool valid_type();

    static void gc_free(void *memory);

    static void *gc_realloc(void *mem, std::size_t size);

    static void gc_lock();

    static void gc_unlock();

    static bool gc_is_lock();

    static bool gc_unload(int id);

    template <typename T, typename X = std::conditional_t<std::is_base_of<Object, T>::value, Object, T>>
    static bool gc_unload(T *pointer);

    static int get_id(void *ptr);

    template <typename T, typename X = std::enable_if_t<std::is_base_of<RoninEngine::Scene, T>::value, T *>>
    static X gc_alloc_scene();

    static Texture *gc_get_texture(int id);

    static SDL_Texture *gc_get_sdl_texture(int id);

    static SDL_Surface *gc_get_sdl_surface(int id);

    static Sprite *gc_get_sprite(int id);

    static bool gc_is_null(int id);

    static std::tuple<int, int> gc_countn();

    static int gc_resource_sdl_surface(const string &resourceName, FolderKind folderKind, SDL_Surface **sdlsurfacePtr);

    static SDL_Surface *gc_resource_sdl_surface(const string &resourceName, FolderKind folderKind) {
        SDL_Surface *sdlsurf;
        gc_resource_sdl_surface(resourceName, folderKind, &sdlsurf);
        return sdlsurf;
    }

    static int gc_alloc_sdl_surface(SDL_Surface **sdlsurfacePtr, const int &w, const int &h);

    static int gc_alloc_sdl_surface(SDL_Surface **sdlsurfacePtr, const int &w, const int &h, const SDL_PixelFormatEnum &format);

    static int gc_alloc_sdl_texture(SDL_Texture **sdltexturePtr, const int &w, const int &h);

    static int gc_alloc_sdl_texture(SDL_Texture **sdltexturePtr, const int &w, const int &h, const SDL_PixelFormatEnum &format);

    static int gc_alloc_sdl_texture(SDL_Texture **sdltexturePtr, const int &w, const int &h, const SDL_PixelFormatEnum &format,
                                    const SDL_TextureAccess &access);

    static int gc_alloc_sdl_texture(SDL_Texture **sdltexturePtr, SDL_Surface *from);

    /// Create Texture with 32x32 pixels, RGBA8888 and access as STATIC
    static int gc_alloc_texture_empty(Texture **texturePtr);

    /// Return allocated id
    static int gc_alloc_texture(Texture **texturePtr, const int &w, const int &h);

    static int gc_alloc_texture(Texture **texturePtr, const int &w, const int &h, const SDL_PixelFormatEnum &format);
    static int gc_alloc_texture(Texture **texturePtr, const int &w, const int &h, const SDL_PixelFormatEnum &format,
                                const SDL_TextureAccess &access);

    static int gc_alloc_texture_from(Texture **texturePtr, SDL_Surface *sdlsurface);

    static int gc_alloc_texture_from(Texture **texturePtr, SDL_Texture *sdltexture);

    /// Create empty Sprite
    static int gc_alloc_sprite_empty(Sprite **spritePtr);

    static int gc_alloc_sprite_empty(Sprite **spritePtr, const Rect_t &rect);

    static int gc_alloc_sprite_with(Sprite **spritePtr, Texture *texture);

    static int gc_alloc_sprite_with(Sprite **spritePtr, Texture *texture, const Vec2 &center);

    static int gc_alloc_sprite_with(Sprite **spritePtr, Texture *texture, const Rect_t &rect, const Vec2 &center = Vec2::half);

    static int gc_alloc_cursor(SDL_Cursor **cursorPtr, SDL_Surface *surface, int hotspot_x, int hotspot_y);

    static int gc_total_allocated();
};
}  // namespace RoninEngine::Runtime
