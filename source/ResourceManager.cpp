#include <filesystem>

#include "ObjectParser.h"
#include "Sprite.h"
#include "pch.h"
#include "scene.h"

using namespace RoninEngine;
using namespace RoninEngine::Runtime;
using namespace RoninEngine::UI;
using namespace RoninEngine::AIPathFinder;

// Pre declaration
#define DECL_OBJECT(X)                                  \
    template X *GC::gc_push<X>();                       \
    template X *GC::gc_push<X>(const std::string &);    \
    template std::list<X> *GC::gc_push<std::list<X>>(); \
    template bool GC::gc_unload<X>(X *);

template MainMenu *GC::gc_alloc_scene<MainMenu>();
template GameScene *GC::gc_alloc_scene<GameScene>();

//---------------------- GC PUSHING & UNLOADING ----------------------
DECL_OBJECT(Object)
DECL_OBJECT(GameObject)
DECL_OBJECT(SpriteRenderer)
DECL_OBJECT(Camera)
DECL_OBJECT(Camera2D)
DECL_OBJECT(Spotlight)
DECL_OBJECT(Player)
DECL_OBJECT(Transform)
DECL_OBJECT(Component)

DECL_OBJECT(char)
DECL_OBJECT(short)
DECL_OBJECT(int)
DECL_OBJECT(float)
DECL_OBJECT(double)
DECL_OBJECT(long double)
DECL_OBJECT(long)
DECL_OBJECT(long long)

template NavMesh *GC::gc_push<NavMesh>();
template NavMesh *GC::gc_push<NavMesh>(const int &, const int &);
template NavMesh *GC::gc_push<NavMesh>(int, int);
template std::list<std::pair<Object *, float>> *GC::gc_push<std::list<std::pair<Object *, float>>>();
template GUI *GC::gc_push<GUI>(Scene*);
template Font_t *GC::gc_push<Font_t>();

template bool GC::gc_unload<Scene>(Scene *);
template bool GC::gc_unload<NavMesh>(NavMesh *);

#undef DECL_OBJECT

// const

constexpr std::uint8_t SDL_TYPE_MAX_INDEX = 31;
constexpr std::uint8_t InvalidType = ~0;

// any type

enum MemoryCapture : std::int8_t { SystemManagement, GCManagement };

struct MemoryStick {
    int memoryType;
    void *memory;
};

template <typename T>
struct type2index {  // undefined types
    static const std::uint8_t typeIndex = InvalidType;
};
template <>
struct type2index<std::remove_pointer_t<void *>> {  // Any pointer
    static const std::uint8_t typeIndex = 0;
};
template <>
struct type2index<SDL_Texture> {  // SDL Texture
    static const std::uint8_t typeIndex = 1;
};
template <>
struct type2index<SDL_Surface> {  // SDL Surface
    static const std::uint8_t typeIndex = 2;
};
template <>
struct type2index<Texture> {  // Ronin Texture
    static const std::uint8_t typeIndex = 32;
};
template <>
struct type2index<Sprite> {  // Ronin Sprite
    static const std::uint8_t typeIndex = 33;
};

/// Object type index
template <typename T>
struct type2index<std::enable_if<std::is_base_of_v<Object, T>, T>> {
    static const std::uint8_t typeIndex = 34;
};

template <typename T>
struct is_sdl_type : public std::integral_constant<bool, type2index<T>::typeIndex <= SDL_TYPE_MAX_INDEX> {};
template <typename T>
struct is_invalid_type : public std::integral_constant<bool, type2index<T>::typeIndex == InvalidType> {};

int lastId;
MemoryCapture memoryCapture = SystemManagement;

vector<MemoryStick> gc_memory[2];

// stringhash and id
map<int, int> *gc_fast_lock;

map<int, list<string>> *_assocMultiFiles;
map<int, list<SDL_Surface *> *> *_assocMultiLoadedImages;
map<list<SDL_Surface *> *, list<Texture *> *> *_assocMultiCacheTextures;

map<int, string> *_assocSingleFile;
map<int, SDL_Surface *> *_resourceLoaded_surfaces;
map<SDL_Surface *, Texture *> *_assocCacheTextures;
map<SDL_Surface *, SDL_Cursor *> *_assocCacheCursors;
list<void *> *_cacheClasses;

map<string, Mix_Music *> *_assocLoadedMusic;

namespace RoninEngine::Runtime {
template <typename T, typename... Args>
constexpr T *_paste_init(T *m, Args &&...args) {
    return new (m) T(std::forward<Args>(args)...);
}

template <typename T>
constexpr T *_deinit(T *m) {
    m->~T();
    return m;
}

template <typename T, typename X = remove_reference_t<remove_pointer_t<T>>>
inline void *gc_alloc_mem() {
    return GC::gc_malloc(sizeof(X));
}

template <typename T, typename X = remove_reference_t<remove_pointer_t<T>>>
inline int gc_wrte_memblock(MemoryStick **ms) {
    int id;

    if (ms == nullptr) throw std::invalid_argument("memory is null");

    vector<MemoryStick> *gc = &gc_memory[static_cast<int>(memoryCapture)];
    *ms = &gc->emplace_back();
    (*ms)->memoryType = type2index<T>::typeIndex;

    //не SDL тип, тогда вызываем memalloc
    if constexpr (!is_sdl_type<T>::value) {
        (*ms)->memory = gc_alloc_mem<X>();
        memset((*ms)->memory, 0, sizeof(X));
    } else {
        // sdl types as nullptr
        (*ms)->memory = nullptr;
    }

    id = gc->size() - 1;  // last index
    return id;
}

bool gc_native_free(MemoryStick *stick) {
    bool result = true;

    if (stick == nullptr || stick->memory == nullptr) return false;

    switch (stick->memoryType) {
        case type2index<SDL_Surface>::typeIndex:
            SDL_FreeSurface(reinterpret_cast<SDL_Surface *>(stick->memory));
            break;
        case type2index<SDL_Texture>::typeIndex:
            SDL_DestroyTexture(reinterpret_cast<SDL_Texture *>(stick->memory));
            break;
        case type2index<Texture>::typeIndex:
            _deinit(reinterpret_cast<Texture *>(stick->memory));
            break;
        case type2index<Sprite>::typeIndex:
            _deinit(reinterpret_cast<Sprite *>(stick->memory));
        case type2index<Object>::typeIndex:
            _deinit(reinterpret_cast<Object *>(stick->memory));
            break;
        default:
            result = false;
    }

    if (result && stick->memoryType > SDL_TYPE_MAX_INDEX) GC::gc_free(stick->memory);

    stick->memoryType = 0;

    return result;
}

int gc_native_collect(const int freeID = -1) {
    int _freed;
    auto gc = &gc_memory[MemoryCapture::GCManagement];  // get management sources

    if (freeID != ~0) {
        return static_cast<int>(static_cast<std::size_t>(freeID) < gc->size() && gc_native_free(&gc->at(freeID)));
    }

    _freed = 0;
    for (auto x : *gc) {
        if (gc_native_free(&x)) ++_freed;
    }

    gc->clear();

    if (gc->capacity() > 1000) {
        gc->reserve(1000);
    }

    return _freed;
}

void GC::gc_init() {
    constexpr int bufferSize = sizeof(*_assocMultiFiles) + sizeof(*_assocMultiLoadedImages) +
                               sizeof(*_assocMultiCacheTextures) + sizeof(*_assocSingleFile) +
                               sizeof(*_resourceLoaded_surfaces) + sizeof(*_assocCacheTextures) + sizeof(*_assocCacheCursors) +
                               sizeof(*_cacheClasses) + sizeof(*_assocLoadedMusic) + sizeof(*gc_fast_lock);

    void *buffer = SDL_malloc(bufferSize);
    memset(buffer, 0, bufferSize);
    _assocMultiFiles = (decltype(_assocMultiFiles))buffer;
    _paste_init(_assocMultiFiles);
    _assocMultiLoadedImages = (decltype(_assocMultiLoadedImages))(_assocMultiFiles + 1);
    _paste_init(_assocMultiLoadedImages);
    _assocMultiCacheTextures = (decltype(_assocMultiCacheTextures))(_assocMultiLoadedImages + 1);
    _paste_init(_assocMultiCacheTextures);
    _assocSingleFile = (decltype(_assocSingleFile))(_assocMultiCacheTextures + 1);
    _paste_init(_assocSingleFile);
    _resourceLoaded_surfaces = (decltype(_resourceLoaded_surfaces))(_assocSingleFile + 1);
    _paste_init(_resourceLoaded_surfaces);
    _assocCacheTextures = (decltype(_assocCacheTextures))(_resourceLoaded_surfaces + 1);
    _paste_init(_assocCacheTextures);
    _assocCacheCursors = (decltype(_assocCacheCursors))(_assocCacheTextures + 1);
    _paste_init(_assocCacheCursors);
    _cacheClasses = (decltype(_cacheClasses))(_assocCacheCursors + 1);
    _paste_init(_cacheClasses);
    _assocLoadedMusic = (decltype(_assocLoadedMusic))(_cacheClasses + 1);
    _paste_init(_assocLoadedMusic);
    gc_fast_lock = (decltype(gc_fast_lock))(_assocLoadedMusic + 1);
    _paste_init(gc_fast_lock);
    lastId = 0;
}
void GC::gc_free() {
    UnloadAll(true);

    _deinit(_assocMultiFiles);
    _deinit(_assocMultiLoadedImages);
    _deinit(_assocMultiCacheTextures);
    _deinit(_assocSingleFile);
    _deinit(_resourceLoaded_surfaces);
    _deinit(_assocCacheTextures);
    _deinit(_assocCacheCursors);
    _deinit(_cacheClasses);
    _deinit(_assocLoadedMusic);

    SDL_free(reinterpret_cast<void *>(_assocMultiFiles));
}

void GC::gc_free_source() {
    gc_native_collect();
    gc_collect();
}
void GC::gc_collect() { free_sdlurl(); }

void GC::LoadImages(const char *filename) {
    ObjectParser parser;

    parser.Deserialize(filename);
    auto data = parser.GetContainer();

    for (auto iter = begin(data); iter != end(data); ++iter) {
        if (!iter->second.isString()) {
            SDL_Log("Image can't load. Incorrect filename in key - %s", iter->second.getPropertyName().data());
            continue;
        }
        if (iter->second.isArray()) {
            list<string> __names;
            for (auto x = iter->second.toStrings()->begin(); x != iter->second.toStrings()->end(); ++x) {
                __names.emplace_back(*x);
            }
            _assocMultiFiles->emplace(make_pair(iter->first, __names));
        } else {
            _assocSingleFile->emplace(make_pair(iter->first, iter->second.toString()));
        }
    }
}
void GC::UnloadUnused() { UnloadAll(false); }
void GC::UnloadAll(bool immediate) {
    // TO-DO: реализовать очистку _assocSingleFile и других

    for (auto xx = begin(*_assocMultiCacheTextures); xx != end(*_assocMultiCacheTextures); ++xx) {
        for (auto iter = begin(*xx->second); iter != end(*xx->second); ++iter) gc_unload(*iter);
        gc_unload(xx->second);
    }

    for (auto xx = begin(*_assocMultiLoadedImages); xx != end(*_assocMultiLoadedImages); ++xx) {
        for (auto iter = begin(*xx->second); iter != end(*xx->second); ++iter) gc_unload((*iter));
        gc_unload(xx->second);
    }

    for (auto iter = begin(*_assocCacheTextures); iter != end(*_assocCacheTextures); ++iter) gc_unload(iter->second);

    for (auto iter : *_cacheClasses) {
        gc_unload(iter);
    }

    _cacheClasses->clear();
    _assocMultiLoadedImages->clear();
    _assocMultiCacheTextures->clear();
    _assocCacheTextures->clear();

    if (immediate) {
        for (auto iter = begin(*_resourceLoaded_surfaces); iter != end(*_resourceLoaded_surfaces); ++iter) {
            if (iter->second->refcount <= 1) SDL_FreeSurface(iter->second);
        }
        _resourceLoaded_surfaces->clear();
    }
}

void GC::CheckResources() {
    string p = dataPath();
    char *membuf = (char *)GC::gc_malloc(PATH_MAX);
    membuf[0] = '\0';
    if (!filesystem::exists(p)) {
        strcat(membuf, "\"Data\" is not found");
        RoninApplication::fail(membuf);
    }
    GC::gc_free(membuf);
}

void *GC::GoMemoryCache(void *cacheToFree) {
    if (find_if(begin(*_cacheClasses), end(*_cacheClasses), [&cacheToFree](auto &x) { return x == cacheToFree; }) ==
        end(*_cacheClasses))
        _cacheClasses->emplace_back(cacheToFree);
    return cacheToFree;
}

//Для автоматического уничтожения ресурса, обязательно его нужно скинуть на
// ResourceManager::Unload()
list<SDL_Surface *> *GC::LoadSurfaces(const string &packName) {
    list<SDL_Surface *> *surfs = nullptr;
    string path = dataAt(FolderKind::GFX);
    string cat;
    int hash = stringToHash(packName.c_str());
    auto iter = _assocMultiLoadedImages->find(hash);
    if (iter == end(*_assocMultiLoadedImages)) {
        auto iBits = _assocMultiFiles->find(hash);
        if (iBits != end(*_assocMultiFiles)) {
            gc_unload(surfs);
            for (auto f = begin(iBits->second); f != end(iBits->second); ++f) {
                cat = path;
                cat += *f;
                surfs->emplace_back(IMG_Load(cat.c_str()));
            }

            _assocMultiLoadedImages->insert(make_pair(hash, surfs));
        }
    } else {
        surfs = iter->second;
    }
    return surfs;
}

//Для автоматического уничтожения ресурса, обязательно его нужно скинуть на
// ResourceManager::Unload()
list<Texture *> *GC::LoadTextures(const string &packName, bool autoUnload) {
    list<Texture *> *_textures = nullptr;
    list<SDL_Surface *> *surfaces = LoadSurfaces(packName);
    Texture *p;
    if (!surfaces || !surfaces->size()) return _textures;

    auto iter = _assocMultiCacheTextures->find(surfaces);
    if (iter == end(*_assocMultiCacheTextures)) {
        GC::gc_push_lvalue(_textures);
        for (auto i = begin(*surfaces); i != end(*surfaces); ++i) {
            gc_alloc_texture_from(&p, SDL_CreateTextureFromSurface(RoninApplication::GetRenderer(), *i));
            _textures->emplace_back(p);
        }
        _assocMultiCacheTextures->emplace(make_pair(surfaces, _textures));
    } else {
        _textures = iter->second;
    }

    return _textures;
}

//Для автоматического уничтожения ресурса, обязательно его нужно скинуть на
// ResourceManager::Unload()
SDL_Surface *GC::GetSurface(const string &surfaceName, FolderKind pathOn) {
    return gc_resource_sdl_surface(surfaceName, pathOn);
}
SDL_Surface *GC::GetSurface(const string &surfaceName) { return GetSurface(surfaceName, FolderKind::GFX); }

//Для автоматического уничтожения ресурса, обязательно его нужно скинуть на
// ResourceManager::Unload()
Texture *GC::GetTexture(const string &resourceName, FolderKind pathOn, bool autoUnload) {
    SDL_Surface *surf;
    Texture *texture;

    surf = gc_resource_sdl_surface(resourceName, pathOn);

    if (!surf) return nullptr;

    auto cache = _assocCacheTextures->find(surf);
    if (cache == end(*_assocCacheTextures)) {
        gc_alloc_texture_from(&texture, surf);
        auto i = _assocCacheTextures->insert(make_pair(surf, texture));
        texture->raw_namePtr = (char *)SDL_malloc(resourceName.size() + 1);
        strcpy(texture->raw_namePtr, resourceName.data());
    } else
        texture = cache->second;

    return texture;
}
Texture *GC::GetTexture(const string &resourceName, bool autoUnload) {
    return GetTexture(resourceName, FolderKind::GFX, autoUnload);
}

// Create texture format RGBA 8888
Texture *GC::GetTexture(const int w, const int h) { return GetTexture(w, h, SDL_PixelFormatEnum::SDL_PIXELFORMAT_RGBA8888); }
Texture *GC::GetTexture(const int w, const int h, const ::SDL_PixelFormatEnum format) {
    return GetTexture(w, h, format, SDL_TextureAccess::SDL_TEXTUREACCESS_STREAMING);
}
Texture *GC::GetTexture(const int w, const int h, const ::SDL_PixelFormatEnum format, const ::SDL_TextureAccess access) {
    Texture *tex;
    gc_alloc_texture(&tex, w, h, format, access);
    if (tex == nullptr) throw bad_alloc();

    return tex;
}

//Для автоматического уничтожения ресурса, обязательно его нужно скинуть на
// ResourceManager::Unload()
SDL_Cursor *GC::GetCursor(const string &resourceName, const point_t &hotspot, bool autoUnload) {
    return GetCursor(GetSurface(resourceName), hotspot);
}

//Для автоматического уничтожения ресурса, обязательно его нужно скинуть на
// ResourceManager::Unload()
SDL_Cursor *GC::GetCursor(SDL_Surface *texture, const point_t &hotspot) {
    SDL_Cursor *cur = nullptr;
    auto find = _assocCacheCursors->find(texture);
    if (texture) {
        if (find != _assocCacheCursors->end())
            return find->second;
        else {
            cur = SDL_CreateColorCursor(texture, hotspot.x, hotspot.y);
            if (cur) _assocCacheCursors->emplace(make_pair(texture, cur));
        }
    }
    return cur;
}

// ------------------------------------------------ GC Implementation

MemoryCapture last_gc_capture;
bool _is_suspending = false;
void GC::suspend_gc() {
    if (_is_suspending) throw std::runtime_error("GC state is suspended");
    _is_suspending = true;
    last_gc_capture = memoryCapture;
    memoryCapture = MemoryCapture::SystemManagement;
}

void GC::continue_gc() {
    if (_is_suspending) {
        memoryCapture = last_gc_capture;
        _is_suspending = false;
    }
}

void *GC::gc_malloc(std::size_t size) { return std::malloc(size); }

template <typename T, typename... Args>
T *GC::gc_push(Args ...arguments) {
    return nullptr;
}

void GC::gc_free(void *memory) { std::free(memory); }

void *GC::gc_realloc(void *mem, std::size_t size) { return std::realloc(mem, size); }

void GC::gc_lock() { memoryCapture = MemoryCapture::SystemManagement; }
void GC::gc_unlock() { memoryCapture = MemoryCapture::GCManagement; }

bool GC::gc_is_lock() { return memoryCapture == MemoryCapture::SystemManagement; }

// GC Resources   ---------------------------------------------------
int GC::gc_resource_sdl_surface(const string &surfaceName, FolderKind pathOn, SDL_Surface **sdlsurfacePtr) {
    SDL_Surface *surf = nullptr;
    string path = dataAt(pathOn);
    string cat;
    MemoryStick *mem;
    int id;

    int hash = stringToHash(surfaceName.c_str());
    auto iter = _resourceLoaded_surfaces->find(hash);
    if (iter == end(*_resourceLoaded_surfaces)) {
        auto iBitsource = _assocSingleFile->find(hash);

        if (iBitsource == end(*_assocSingleFile)) return GCInvalidID;

        cat = path + iBitsource->second;

        if (!filesystem::exists(cat.c_str())) {
            SDL_LogError(SDL_LOG_CATEGORY_INPUT, "Texture not found. %s", cat.c_str());
            return GCInvalidID;
        }

        surf = IMG_Load(cat.c_str());
        if (!surf) {
            SDL_LogError(SDL_LOG_CATEGORY_INPUT, "Invalid texture. %s", cat.c_str());
            return GCInvalidID;
        }

        id = gc_wrte_memblock<SDL_Surface>(&mem);
        mem->memory = surf;
        _resourceLoaded_surfaces->insert(make_pair(hash, surf));
    } else {
        // finded
        surf = iter->second;
        id = get_id(surf);
    }

    if (sdlsurfacePtr != nullptr) *sdlsurfacePtr = surf;

    return id;
}

// GC SDL Surface ---------------------------------------------------

int GC::gc_alloc_sdl_surface(SDL_Surface **sdlsurfacePtr, const int &w, const int &h) {
    return gc_alloc_sdl_surface(sdlsurfacePtr, w, h, SDL_PixelFormatEnum::SDL_PIXELFORMAT_RGBA8888);
}

int GC::gc_alloc_sdl_surface(SDL_Surface **sdlsurfacePtr, const int &w, const int &h, const SDL_PixelFormatEnum &format) {
    SDL_CreateRGBSurfaceWithFormat(0, w, h, 32, format);
}

Texture *GC::gc_get_texture(int id) { return nullptr; }

Sprite *GC::gc_get_sprite(int id) { return nullptr; }

bool GC::gc_unload(int id) { return gc_native_collect(id) != 0; }

template <typename T, typename X>
bool GC::gc_unload(T *pointer) {
    int id;

    id = get_id(pointer);
    gc_native_collect(id);

    return id != GCInvalidID;
}

int GC::get_id(void *ptr) {
    std::size_t x;
    auto gc = &gc_memory[static_cast<int>(memoryCapture)];
    for (x = 0; x < gc->size(); ++x) {
        if (gc->at(x).memory == ptr) return x;
    }

    return GCInvalidID;
}

// GC Texture---------------------

int GC::gc_alloc_sdl_texture(SDL_Texture **sdltexturePtr, const int &w, const int &h) {
    return gc_alloc_sdl_texture(sdltexturePtr, w, h, SDL_PixelFormatEnum::SDL_PIXELFORMAT_RGBA8888);
}

int GC::gc_alloc_sdl_texture(SDL_Texture **sdltexturePtr, const int &w, const int &h, const SDL_PixelFormatEnum &format) {
    return gc_alloc_sdl_texture(sdltexturePtr, w, h, format, SDL_TextureAccess::SDL_TEXTUREACCESS_STATIC);
}

int GC::gc_alloc_sdl_texture(SDL_Texture **sdltexturePtr, const int &w, const int &h, const SDL_PixelFormatEnum &format,
                             const SDL_TextureAccess &access) {
    int id;
    MemoryStick *mem;

    id = gc_wrte_memblock<SDL_Texture>(&mem);
    auto &&gc_ptr =
        reinterpret_cast<SDL_Texture *>(mem->memory = SDL_CreateTexture(RoninApplication::GetRenderer(), format, access, w, h));

    if (sdltexturePtr != nullptr) (*sdltexturePtr) = gc_ptr;

    return id;
}

int GC::gc_alloc_sdl_texture(SDL_Texture **sdltexturePtr, SDL_Surface *from) {
    int id;
    MemoryStick *mem;

    id = gc_wrte_memblock<SDL_Texture>(&mem);
    mem->memory = SDL_CreateTextureFromSurface(RoninApplication::GetRenderer(), from);
    auto gc_ptr = reinterpret_cast<SDL_Texture *>(mem->memory);

    if (gc_ptr == nullptr) RoninApplication::fail_OutOfMemory();

    if (sdltexturePtr != nullptr) (*sdltexturePtr) = gc_ptr;

    return id;
}

int GC::gc_alloc_texture_empty(Texture **texturePtr) { return gc_alloc_texture(texturePtr, 32, 32); }

int GC::gc_alloc_texture(Texture **texturePtr, const int &w, const int &h) {
    return gc_alloc_texture(texturePtr, w, h, SDL_PixelFormatEnum::SDL_PIXELFORMAT_RGBA8888);
}
int GC::gc_alloc_texture(Texture **texturePtr, const int &w, const int &h, const SDL_PixelFormatEnum &format) {
    return gc_alloc_texture(texturePtr, w, h, SDL_PixelFormatEnum::SDL_PIXELFORMAT_RGBA8888,
                            SDL_TextureAccess::SDL_TEXTUREACCESS_STATIC);
}
int GC::gc_alloc_texture(Texture **texturePtr, const int &w, const int &h, const SDL_PixelFormatEnum &format,
                         const SDL_TextureAccess &access) {
    int id;
    MemoryStick *mem;

    id = gc_wrte_memblock<Texture>(&mem);
    auto gc_ptr = reinterpret_cast<Texture *>(mem->memory);
    if (texturePtr != nullptr) (*texturePtr) = gc_ptr;

    _paste_init(gc_ptr);

    // create SDL Texture (Native) and set to Texture
    gc_alloc_sdl_texture(&gc_ptr->m_native, w, h, format, access);
    return id;
}

int GC::gc_alloc_texture_from(Texture **texturePtr, SDL_Surface *from) {
    int id;
    MemoryStick *mem;

    // alloc empty ptr
    // and create SDL_CreateTextureFromSurface
    id = gc_wrte_memblock<SDL_Texture>(&mem);
    mem->memory = SDL_CreateTextureFromSurface(RoninApplication::GetRenderer(), from);
    auto gc_ptr = reinterpret_cast<SDL_Texture *>(mem->memory);

    gc_alloc_texture_from(texturePtr, gc_ptr);
    return id;
}

int GC::gc_alloc_texture_from(Texture **texturePtr, SDL_Texture *sdltexture) {
    int id;
    MemoryStick *mem;

    id = gc_wrte_memblock<Texture>(&mem);
    auto gc_ptr = reinterpret_cast<Texture *>(mem->memory);

    _paste_init(gc_ptr);
    gc_ptr->m_native = sdltexture;

    if (texturePtr != nullptr) (*texturePtr) = reinterpret_cast<Texture *>(gc_ptr);

    return id;
}

// GC Sprite---------------------

int GC::gc_alloc_sprite_empty(Sprite **spritePtr) { return gc_alloc_sprite_empty(spritePtr, {0, 0, 0, 0}); }

int GC::gc_alloc_sprite_empty(Sprite **spritePtr, const Rect_t &rect) {
    return gc_alloc_sprite_with(spritePtr, nullptr, rect, Vec2::half);
}

int GC::gc_alloc_sprite_with(Sprite **spritePtr, Texture *texture) {
    return gc_alloc_sprite_with(spritePtr, texture, Vec2::half);
}

int GC::gc_alloc_sprite_with(Sprite **spritePtr, Texture *texture, const Vec2 &center) {
    Rect_t rect{};
    if (texture == nullptr) {
        static_assert(true, "texture arg is null");
    } else {
        rect.w = texture->width();
        rect.h = texture->height();
    }

    return gc_alloc_sprite_with(spritePtr, texture, rect, center);
}

int GC::gc_alloc_sprite_with(Sprite **spritePtr, Texture *texture, const Rect_t &rect, const Vec2 &center) {
    int id;
    MemoryStick *ms;

    id = gc_wrte_memblock<Sprite>(&ms);
    auto gc_ptr = reinterpret_cast<Sprite *>(ms->memory);

    gc_ptr->texture = texture;
    gc_ptr->m_center = center;
    gc_ptr->m_rect = rect;

    if (spritePtr != nullptr) {
        *spritePtr = gc_ptr;
    }

    return id;
}

template <typename T, typename X>
X GC::gc_alloc_scene() {
    MemoryStick *m;
    int id;
    MemoryCapture &&last = std::move(memoryCapture);
    memoryCapture = MemoryCapture::SystemManagement;
    id = gc_wrte_memblock<X>(&m);
    memoryCapture = last;

    return _paste_init(reinterpret_cast<X>(m->memory));
}

// ------------------------------------------------------- GC Stats (Total N)

std::tuple<int, int> GC::gc_countn() { return std::make_tuple(gc_memory[0].size(), gc_memory[1].size()); }

int GC::gc_total_allocated() {
    int n;

    tuple<int, int> ref = gc_countn();
    n = get<1>(ref);

    return n;
}

template <typename T>
bool GC::valid_type() {
    return type2index<T>::typeIndex != InvalidType;
}

}  // namespace RoninEngine::Runtime
