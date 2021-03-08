#pragma once

#include "framework.h"

using namespace std;

namespace RoninEngine::Runtime {
   enum Resourceid : std::uint8_t{
      RESERVED0,
      RESERVED1,
      RESERVED2,
      RESERVED3,
      RESERVED4,
      RESERVED5,
      RESERVED6,
      RESERVED7,
      RID_LOCALE_RU,
      RID_LOCALE_EN
   };

class ResourceManager {
    friend class ::RoninEngine::RoninApplication;
    friend class Scene;
    template <typename T, typename... Types>
    friend constexpr T* ::allocate_class(Types&&... _Args);

   protected:
    static void ResourcesInitialize();
    static void ResourcesRelease();
    static void UnloadAll(bool immediate);
    static void CheckResources();

   public:
    [[deprecate]] static void LoadImages(const char* filename);
    [[deprecate]] static void UnloadUnused();
    [[deprecate]] static void* GoMemoryCache(void* cacheToFree);

    [[deprecate]] static list<SDL_Surface*>* LoadSurfaces(
        const string& packName);
    [[deprecate]] static list<Runtime::Texture*>* LoadTextures(
        const string& packName, bool autoUnload = true);
    [[deprecate]] static SDL_Surface* GetSurface(const string& surfaceName);
    [[deprecate]] static SDL_Surface* GetSurface(const string& surfaceName,
                                                 FolderKind pathOn);
    [[deprecate]] static Texture* GetTexture(const string& resourceName,
                                             FolderKind pathOn,
                                             bool autoUnload = true);
    [[deprecate]] static Texture* GetTexture(const string& resourceName,
                                             bool autoUnload = true);
    [[deprecate]] static Texture* GetTexture(const int w, const int h);
    [[deprecate]] static Texture* GetTexture(
        const int w, const int h, const ::SDL_PixelFormatEnum format);
    [[deprecate]] static Texture* GetTexture(const int w, const int h,
                                             const ::SDL_PixelFormatEnum format,
                                             const ::SDL_TextureAccess access);
    [[deprecate]] static SDL_Cursor* GetCursor(const string& resourceName,
                                               const Vector2i& hotspot,
                                               bool autoUnload = true);
    [[deprecate]] static SDL_Cursor* GetCursor(SDL_Surface* texture,
                                               const Vector2i& hotspot);

    [[deprecate]] static Atlas* GetAtlas(const string& atlasName);
    [[deprecate]] static Sprite* GetSprite(const string& spriteName);
    [[deprecate]] static Sprite* GetSprite(int w, int h);
    [[deprecate]] static Sprite* GetSprite(Texture* attach);

    [[deprecate]] static bool UnloadTexture(Texture* const texture);
    [[deprecate]] static void Unload(SDL_Cursor* unload);
    [[deprecate]] static void Unload(SDL_Surface* unload);

    static int load_atlas(Atlas* source);

    static Atlas* get_atlas(int id);
    static Sprite get_sprite(int id);

    static string& get_string(int id);
    static std::int64_t& get_number(int id);
    static double& get_real(int id);
    static SDL_Cursor* get_cursor(int id);
    static Texture* get_texture(int id);

    static SDL_Surface* get_surface(int id);

    static void* get_pointer(int id);

    static int totaln();
    static int total_memory();
};
}  // namespace RoninEngine::Runtime
