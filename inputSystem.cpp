#include "pch.h"
#include "inputSystem.h"

namespace RoninEngine
{
	Vector2i input::m_mousePoint;
	char input::mouseState = 0, input::lastMouseState, input::mouseWheels = 0;
	SDL_Event* input::event;
	Vec2 input::m_axis;
}