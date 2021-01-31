#ifndef ___framework_H
#define ___framework_H

#include <stdio.h>
#include <string>
#include <vector>
#include <list>
#include <algorithm>
#include <new>
#include <memory>
#include <list>
#include <stdint.h>
#include <map>
#include <stdexcept>

/*
#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"
#include "SDL2/SDL_mixer.h"
*/
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>


#include "../../../../LIBS-Data/dev/modules/queue_list.h"
#include "../../../../LIBS-Data/dev/modules/config_fastfile.h"

#include "reference.h"
#include "global_val.h"
#include "time.h"
#include "paths.h"
#include "types.h"
#include "font.h"
#include "Color.h"
#include "Texture.h"

template<typename T>
constexpr void deallocate_class(T* var)
{
	var->~T();
	SDL_free(var);
}

template<typename T>
constexpr void deallocate_variable(T*& var)
{
	var->~T();
	SDL_free(var);
	var = NULL;
}

template<typename T, typename... Types>
constexpr T* allocate_class(Types&&... _Args) {
	T* allocated = (T*)SDL_malloc(sizeof(T));
	SDL_memset(allocated, 0, sizeof(T));
	new(allocated)T(std::forward<Types>(_Args)...);
	return allocated;
}

template<typename T>
constexpr T* allocate_class() {
	T* allocated = (T*)SDL_malloc(sizeof(T));
	SDL_memset(allocated, 0, sizeof(T));
	new(allocated)T();
	return allocated;
}

template<typename T>
constexpr T*& allocate_variable(T*& var) {
	return var = allocate_class<remove_reference<T>::type>();
}

template<typename T, typename... _Types>
constexpr T*& allocate_variable(T*& var, _Types&&... _Args) {
	return var = allocate_class<remove_reference<T>::type>(forward<_Types>(_Args)...);
}

template<typename T>
constexpr T*& reset_variable(T*& var) {
	deallocate_variable(var);
	return allocate_variable(var);
}

template<typename T, typename... _Types>
constexpr T*& reset_variable(T*& var, _Types&&... _Args)
{
	deallocate_variable(var);
	return allocate_variable(var, _Args);
}



template <typename _Container, typename _Pred>
constexpr void Foreach(_Container& _cont, _Pred _Fn)
{
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

		struct Color;
	}
}

#endif 
