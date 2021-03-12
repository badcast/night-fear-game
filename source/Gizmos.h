#pragma once
#include "framework.h"
#include "NavMesh.h"

namespace RoninEngine::Runtime {
	class Gizmos
	{
	public:
        static Color color;
        static Vec2 offset;
        static float angle;

        static void DrawLine(Vec2 a, Vec2 b);
		static void DrawPosition(Vec2 origin);
		static void Draw2DWorldSpace(Vec2 origin);
		static void DrawNavMesh(AIPathFinder::NavMesh* map);
        static void DrawTriangle(Vec2 pos, float base, float height);

        static float square_triangle(float base, float height);
        static float square(float x);
        static float square_rectangle(float a, float b);
        static float square_circle(float radius, float circle);
	};
}

