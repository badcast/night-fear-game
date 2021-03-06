#pragma once

#include "framework.h"
#include "Component.h"

namespace RoninEngine::Runtime
{
	struct Render_info
	{
          Rect_t src;
          Rectf_t dst;
          SDL_Renderer* renderer;
          Texture* texture;
	};

	class Renderer : public Component
	{
	public:
		Renderer();
		Renderer(const string& name);
		Renderer(const Renderer&) = delete;

		virtual Vec2 GetSize() = 0;

		virtual void Render(Render_info* render_info) = 0;
		uint8_t zOrder;
	}; 
}

