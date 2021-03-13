#include <filesystem>

#include "ObjectParser.h"
#include "Sprite.h"
#include "pch.h"

map<int, list<string>>* _assocMultiFiles;
map<int, list<SDL_Surface*>*>* _assocMultiLoadedImages;
map<list<SDL_Surface*>*, list<Texture*>*>* _assocMultiCacheTextures;

map<int, string>* _assocSingleFile;
map<int, SDL_Surface*>* _assocLoadedImages;
map<SDL_Surface*, Texture*>* _assocCacheTextures;
map<SDL_Surface*, SDL_Cursor*>* _assocCacheCursors;
list<void*>* _cacheClasses;

map<string, Mix_Music*>* _assocLoadedMusic;

vector<Texture*>* _unusedTextures;
vector<Sprite*>* _usedSprites;

enum MemoryCapture : std::int8_t { SystemManagement, SceneManagement };

struct MemoryBlock {
    int memoryType;
    void* memory;
};

MemoryCapture memoryCapture = SystemManagement;

list<MemoryBlock> gc_memory[2];

namespace RoninEngine::Runtime {
template <typename T>
[[deprecated]] constexpr T* _paste_init(T* m) {
    return new (m) T();
}

template <typename T>
[[deprecated]] constexpr T* _deinit(T* m) {
    m->~T();
    return m;
}

void ResourceManager::ResourcesInitialize() {
    constexpr int bufferSize =
        sizeof(*_assocMultiFiles) + sizeof(*_assocMultiLoadedImages) +
        sizeof(*_assocMultiCacheTextures) + sizeof(*_assocSingleFile) +
        sizeof(*_assocLoadedImages) + sizeof(*_assocCacheTextures) +
        sizeof(*_assocCacheCursors) + sizeof(*_cacheClasses) +
        sizeof(*_assocLoadedMusic) + sizeof(*_unusedTextures) +
        sizeof(*_usedSprites);

    void* buffer = SDL_malloc(bufferSize);
    memset(buffer, 0, bufferSize);
    _assocMultiFiles = (decltype(_assocMultiFiles))buffer;
    _paste_init(_assocMultiFiles);
    _assocMultiLoadedImages =
        (decltype(_assocMultiLoadedImages))(_assocMultiFiles + 1);
    _paste_init(_assocMultiLoadedImages);
    _assocMultiCacheTextures =
        (decltype(_assocMultiCacheTextures))(_assocMultiLoadedImages + 1);
    _paste_init(_assocMultiCacheTextures);
    _assocSingleFile =
        (decltype(_assocSingleFile))(_assocMultiCacheTextures + 1);
    _paste_init(_assocSingleFile);
    _assocLoadedImages = (decltype(_assocLoadedImages))(_assocSingleFile + 1);
    _paste_init(_assocLoadedImages);
    _assocCacheTextures =
        (decltype(_assocCacheTextures))(_assocLoadedImages + 1);
    _paste_init(_assocCacheTextures);
    _assocCacheCursors =
        (decltype(_assocCacheCursors))(_assocCacheTextures + 1);
    _paste_init(_assocCacheCursors);
    _cacheClasses = (decltype(_cacheClasses))(_assocCacheCursors + 1);
    _paste_init(_cacheClasses);
    _assocLoadedMusic = (decltype(_assocLoadedMusic))(_cacheClasses + 1);
    _paste_init(_assocLoadedMusic);
    _unusedTextures = (decltype(_unusedTextures))(_assocLoadedMusic + 1);
    _paste_init(_unusedTextures);
    _usedSprites = (decltype(_usedSprites))(_unusedTextures + 1);
    _paste_init(_usedSprites);
}
void ResourceManager::ResourcesRelease() {
    UnloadAll(true);

    _deinit(_assocMultiFiles);
    _deinit(_assocMultiLoadedImages);
    _deinit(_assocMultiCacheTextures);
    _deinit(_assocSingleFile);
    _deinit(_assocLoadedImages);
    _deinit(_assocCacheTextures);
    _deinit(_assocCacheCursors);
    _deinit(_cacheClasses);
    _deinit(_assocLoadedMusic);
    _deinit(_unusedTextures);
    _deinit(_usedSprites);

    SDL_free(reinterpret_cast<void*>(_assocMultiFiles));
}

void ResourceManager::LoadImages(const char* filename) {
    ObjectParser parser;

    parser.Deserialize(filename);
    auto data = parser.GetContainer();

    for (auto iter = begin(data); iter != end(data); ++iter) {
        if (!iter->second.isString()) {
            SDL_Log("Image can't load. Incorrect filename in key - %s",
                    iter->second.getPropertyName().data());
            continue;
        }
        if (iter->second.isArray()) {
            list<string> __names;
            for (auto x = iter->second.toStrings()->begin();
                 x != iter->second.toStrings()->end(); ++x) {
                __names.emplace_back(*x);
            }
            _assocMultiFiles->emplace(make_pair(iter->first, __names));
        } else {
            _assocSingleFile->emplace(
                make_pair(iter->first, iter->second.toString()));
        }
    }
}
void ResourceManager::UnloadUnused() { UnloadAll(false); }
void ResourceManager::UnloadAll(bool immediate) {
    // TO-DO: реализовать очистку _assocSingleFile и других

    for (auto xx = begin(*_assocMultiCacheTextures);
         xx != end(*_assocMultiCacheTextures); ++xx) {
        for (auto iter = begin(*xx->second); iter != end(*xx->second); ++iter)
            deallocate_class(*iter);
        deallocate_class(xx->second);
    }

    for (auto xx = begin(*_assocMultiLoadedImages);
         xx != end(*_assocMultiLoadedImages); ++xx) {
        for (auto iter = begin(*xx->second); iter != end(*xx->second); ++iter)
            SDL_FreeSurface((*iter));
        deallocate_class(xx->second);
    }

    for (auto iter = begin(*_assocCacheTextures);
         iter != end(*_assocCacheTextures); ++iter)
        deallocate_class(iter->second);

    for (auto iter : *_cacheClasses) {
        deallocate_class(iter);
    }
    for (auto iter : *_unusedTextures) {
        deallocate_class(iter);
    }
    for (auto iter : *_usedSprites) {
        deallocate_class(iter);
    }

    _cacheClasses->clear();
    _assocMultiLoadedImages->clear();
    _assocMultiCacheTextures->clear();
    _assocCacheTextures->clear();
    _unusedTextures->clear();
    _unusedTextures->shrink_to_fit();
    _usedSprites->clear();
    _usedSprites->shrink_to_fit();

    if (immediate) {
        for (auto iter = begin(*_assocLoadedImages);
             iter != end(*_assocLoadedImages); ++iter) {
            if (iter->second->refcount <= 1) SDL_FreeSurface(iter->second);
        }
        _assocLoadedImages->clear();
    }
}

void ResourceManager::CheckResources() {
    string p = dataPath();
    char* membuf = (char*)SDL_malloc(25);
    membuf[0] = '\0';
    if (!filesystem::exists(p)) {
        strcat(membuf, "\"Data\" is not found");
        RoninApplication::fail(membuf);
    }
    SDL_free(membuf);
}

void* ResourceManager::GoMemoryCache(void* cacheToFree) {
    if (find_if(begin(*_cacheClasses), end(*_cacheClasses),
                [&cacheToFree](auto& x) { return x == cacheToFree; }) ==
        end(*_cacheClasses))
        _cacheClasses->emplace_back(cacheToFree);
    return cacheToFree;
}

//Для автоматического уничтожения ресурса, обязательно его нужно скинуть на
// ResourceManager::Unload()
list<SDL_Surface*>* ResourceManager::LoadSurfaces(const string& packName) {
    list<SDL_Surface*>* surfs = NULL;
    string path = dataAt(FolderKind::GFX);
    string cat;
    int hash = stringToHash(packName.c_str());
    auto iter = _assocMultiLoadedImages->find(hash);
    if (iter == end(*_assocMultiLoadedImages)) {
        auto iBits = _assocMultiFiles->find(hash);
        if (iBits != end(*_assocMultiFiles)) {
            allocate_variable(surfs);
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
list<Texture*>* ResourceManager::LoadTextures(const string& packName,
                                              bool autoUnload) {
    list<Texture*>* _textures = NULL;
    list<SDL_Surface*>* surfaces = LoadSurfaces(packName);

    if (!surfaces || !surfaces->size()) return _textures;

    auto iter = _assocMultiCacheTextures->find(surfaces);
    if (iter == end(*_assocMultiCacheTextures)) {
        allocate_variable(_textures);
        for (auto i = begin(*surfaces); i != end(*surfaces); ++i)
            _textures->emplace_back(
                allocate_class<Texture>(SDL_CreateTextureFromSurface(
                    RoninEngine::UI::guiInstance->renderer, *i)));
        _assocMultiCacheTextures->emplace(make_pair(surfaces, _textures));
    } else {
        _textures = iter->second;
    }

    return _textures;
}

//Для автоматического уничтожения ресурса, обязательно его нужно скинуть на
// ResourceManager::Unload()
SDL_Surface* ResourceManager::GetSurface(const string& surfaceName,
                                         FolderKind pathOn) {
    SDL_Surface* surf = NULL;
    string path = dataAt(pathOn);
    string cat;
    int hash = stringToHash(surfaceName.c_str());
    auto iter = _assocLoadedImages->find(hash);
    if (iter == end(*_assocLoadedImages)) {
        auto iBitsource = _assocSingleFile->find(hash);

        if (iBitsource == end(*_assocSingleFile)) return nullptr;

        cat = path + iBitsource->second;

        if (!filesystem::exists(cat.c_str())) {
            SDL_LogError(SDL_LOG_CATEGORY_INPUT, "Texture not found. %s",
                         cat.c_str());
            return NULL;
        }

        surf = IMG_Load(cat.c_str());
        if (!surf) {
            SDL_LogError(SDL_LOG_CATEGORY_INPUT, "Invalid texture. %s",
                         cat.c_str());
            return NULL;
        }

        _assocLoadedImages->insert(make_pair(hash, surf));
    } else {
        surf = iter->second;
    }
    return surf;
}
SDL_Surface* ResourceManager::GetSurface(const string& surfaceName) {
    return GetSurface(surfaceName, FolderKind::GFX);
}

//Для автоматического уничтожения ресурса, обязательно его нужно скинуть на
// ResourceManager::Unload()
Texture* ResourceManager::GetTexture(const string& resourceName,
                                     FolderKind pathOn, bool autoUnload) {
    SDL_Surface* surf;
    Texture* texture;

    surf = GetSurface(resourceName, pathOn);

    if (!surf) return NULL;

    auto cache = _assocCacheTextures->find(surf);
    if (cache == end(*_assocCacheTextures)) {
        auto i = _assocCacheTextures->insert(make_pair(
            surf,
            (texture = allocate_class<Texture>(SDL_CreateTextureFromSurface(
                 RoninApplication::GetRenderer(), surf)))));
        texture->raw_namePtr = (char*)SDL_malloc(resourceName.size() + 1);
        strcpy(texture->raw_namePtr, resourceName.data());
    } else
        texture = cache->second;

    return texture;
}
Texture* ResourceManager::GetTexture(const string& resourceName,
                                     bool autoUnload) {
    return GetTexture(resourceName, FolderKind::GFX, autoUnload);
}

// Create texture format RGBA 8888
Texture* ResourceManager::GetTexture(const int w, const int h) {
    return GetTexture(w, h, SDL_PixelFormatEnum::SDL_PIXELFORMAT_RGBA8888);
}
Texture* ResourceManager::GetTexture(const int w, const int h,
                                     const ::SDL_PixelFormatEnum format) {
    return GetTexture(w, h, format,
                      SDL_TextureAccess::SDL_TEXTUREACCESS_STREAMING);
}
Texture* ResourceManager::GetTexture(const int w, const int h,
                                     const ::SDL_PixelFormatEnum format,
                                     const ::SDL_TextureAccess access) {
    Texture* tex;
    allocate_variable(tex, w, h, format, access);
    _unusedTextures->emplace_back(tex);
    return tex;
}

//Для автоматического уничтожения ресурса, обязательно его нужно скинуть на
// ResourceManager::Unload()
SDL_Cursor* ResourceManager::GetCursor(const string& resourceName,
                                       const Vector2i& hotspot,
                                       bool autoUnload) {
    return GetCursor(GetSurface(resourceName), hotspot);
}

//Для автоматического уничтожения ресурса, обязательно его нужно скинуть на
// ResourceManager::Unload()
SDL_Cursor* ResourceManager::GetCursor(SDL_Surface* texture,
                                       const Vector2i& hotspot) {
    SDL_Cursor* cur = NULL;
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

Sprite* ResourceManager::GetSprite(const string& spriteName) { return NULL; }

Sprite* ResourceManager::GetSprite(Texture* attach) {
    Sprite* spr;
    allocate_variable(spr, attach);
    _usedSprites->emplace_back(spr);
    return spr;
}

Sprite* ResourceManager::GetSprite(int w, int h) {
    Sprite* spr;
    allocate_variable(spr, reinterpret_cast<Texture*>(NULL),
                      Rect_t(0, 0, w, h));
    _usedSprites->push_back(spr);
    return spr;
}

bool ResourceManager::UnloadTexture(Texture* const texturePtr) {
    if (!texturePtr || !texturePtr->raw_namePtr) {
        fprintf(stderr, "texturePtr is null");
    } else {
        SDL_Surface* sfPtr =
            GetSurface(string(texturePtr->raw_namePtr), FolderKind::TEXTURES);

        if (!sfPtr)
            fprintf(stderr, "Texture is shared memory group");
        else {
            _assocCacheTextures->erase(sfPtr);
            // deallocate_class(texturePtr);

            auto iter =
                find_if(_unusedTextures->begin(), _unusedTextures->end(),
                        [texturePtr](Texture* f) { return f == texturePtr; });

            if (iter == _unusedTextures->end()) {
                _unusedTextures->emplace_back(texturePtr);
            }

            return true;
        }
    }
    return false;
}

void ResourceManager::Unload(SDL_Cursor* unload) {
    // todo: для курсоров
}
void ResourceManager::Unload(SDL_Surface* unload) { --unload->refcount; }

void ResourceManager::system_capture() {
    memoryCapture = MemoryCapture::SystemManagement;
}
void ResourceManager::gc_capture() {
    memoryCapture = MemoryCapture::SystemManagement;
}
}  // namespace RoninEngine::Runtime
