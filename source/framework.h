#ifndef ___framework_H
#define ___framework_H

#include <cstdio>
#include <string>
#include <vector>
#include <list>
#include <algorithm>
#include <new>
#include <memory>
#include <list>
#include <cstdint>
#include <map>
#include <stdexcept>
#include <typeinfo>
#include <cassert>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>

//libx/dev/modules/
#include "../../../libx/dev/modules/queue_list.h"
#include "../../../libx/dev/modules/config_fastfile.h"

//#include "reference.h"
#include "global_val.h"
#include "time.h"
#include "paths.h"
#include "types.h"
#include "font.h"
#include "Color.h"
#include "Texture.h"


template<typename T>
[[deprecated]]
constexpr void free_variable(T* var)
{
    static_assert("free_variable");
    var->~T();
    SDL_free(var);
}

template<typename T>
[[deprecated]]
constexpr void deallocate_class(T* var)
{
   static_assert("deallocated_class");
   if constexpr (!std::is_same<T*, void*>::value)
   {
     var->~T();
   }
   SDL_free(var);
}

template<typename T>
[[deprecated]]
constexpr void deallocate_variable(T*& var)
{
   static_assert("deallocate_variable");
    var->~T();
    SDL_free(var);
    var = nullptr;
}

template<typename T, typename... Types>
[[deprecated]]
constexpr T* allocate_class(Types&&... _Args) {
    static_assert("allocate_class+_Args...");
    T* allocated = reinterpret_cast<T*>(SDL_malloc(sizeof(T)));
    SDL_memset(allocated, 0, sizeof(T));
    new(allocated)T(std::forward<Types>(_Args)...);
    return allocated;
}

template<typename T, typename... Types>
[[deprecated]]
constexpr T*& allocate_variable(T*& var, Types&&... _Args) {
     static_assert("allocate_variable+var+_Args...");
    return var = allocate_class<std::remove_reference_t<T>>(std::forward<Types>(_Args)...);
}

template<typename T>
[[deprecated]]
constexpr T*& reset_variable(T*& var) {
     static_assert("reset_variable+var");
    free_variable(var);
    return allocate_variable(var);
}

template<typename T, typename... Types>
[[deprecated]]
constexpr T*& reset_variable(T*& var, Types&&... vars)
{
     static_assert("reset_variable+var+vars...");
    free_variable(var);
    throw std::bad_exception();
    return 0;//allocate_variable(var, std::forward(vars));
}

template <typename _Container, typename _Pred>
[[deprecated]]
constexpr void Foreach(_Container& _cont, _Pred _Fn)
{
    static_assert("Foreach+fucked...");
	for (auto iter = begin(_cont); iter != end(_cont); ++iter)
		_Fn(*iter);
}

namespace RoninEngine {
	class RoninApplication;
	class Scene;

    //Pre declaration
	namespace Runtime
	{
		class Object;
		class Component;
		class Renderer;
		class SpriteRenderer;
		class Transform;
		class Camera;
		class Camera2D;
		class GameObject;
		class Texture;
		class Atlas;
		class Sprite;

        class Light;

        class Behaviour;

		struct Color;
	}
}

#endif 
