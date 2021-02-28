#pragma once

#include "framework.h"
#include "Renderer.h"
#include "SpriteRenderer.h"

namespace RoninEngine::Runtime
{
	class Atlas;

	class Sprite
	{
		friend class Scene;
		friend class Camera2D;
		friend class Renderer;
		friend class SpriteRenderer;
		friend class Atlas;
		friend class ResourceManager;
		template<typename T, typename... Types>
		friend constexpr T* ::allocate_class(Types&&... _Args);		
		template<typename T, typename ... _Types>
		friend constexpr T*& ::allocate_variable(T*& var, _Types&&... _Args);

	protected:
		Vec2 m_center;
		Rect_t m_rect;

		Sprite(Texture* texture);
		Sprite(Texture* texture, Vec2 center);
		Sprite(Texture* texture, Rect_t m_rect, Vec2 center = Vec2(.5f, .5f));

	public:
		Texture* texture;

		Sprite(const Sprite&) = default;

		const Rect_t Rect();
		void Rect(Rect_t rect);

		const Vec2 Center();
		void Center(Vec2 center);

		const bool valid();

		int width();
		int height();

		static Sprite* empty();
	};


}
