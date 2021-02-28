#pragma once
#include "framework.h"
#include "NavMesh.h"

namespace RoninEngine::Runtime {
	class Gizmos
	{
	public:
		static SDL_Color color;

		static void DrawLine(Vec2 a, Vec2 b);
		static void DrawPosition(Vec2 origin);
		static void Draw2DWorldSpace(Vec2 origin);
		static void DrawNavMesh(AIPathFinder::NavMesh* map);
	};
}

