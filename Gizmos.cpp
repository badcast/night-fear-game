#include "pch.h"
#include "Gizmos.h"

namespace RoninEngine::Runtime {
	float maxWorldScalar = 10000;

	SDL_Color Gizmos::color;

	void Gizmos::DrawLine(Vec2 a, Vec2 b) {
		if (!Camera::mainCamera())
			return;
		Vec2 p = Camera::mainCamera()->transform()->position();
		Vec2 dst;
		auto display = RoninApplication::instance()->display();

		//dst.x = ((rect.w - dst.w) / 2.0f - (point->x + sourcePoint->x) * squarePerPixels);
		//dst.y = ((rect.h - dst.h) / 2.0f + (point->y - sourcePoint->y) * squarePerPixels);

		//scalars

		dst.x = display.w / 2.f;
		dst.y = display.h / 2.f;
		a.x = dst.x - (p.x - a.x) * squarePerPixels;
		a.y = dst.y + (p.y - a.y) * squarePerPixels;
		b.x = dst.x - (p.x - b.x) * squarePerPixels;
		b.y = dst.y + (p.y - b.y) * squarePerPixels;

		SDL_SetRenderDrawColor(RoninApplication::instance()->GetRenderer(), color.r, color.g, color.b, color.a);
		SDL_RenderDrawLineF(RoninApplication::instance()->GetRenderer(), a.x, a.y, b.x, b.y);
	}
	void drawBox() {
		Vec2 a, b;

		a.x = squarePerPixels;
		a.y = squarePerPixels;
		b.x = -squarePerPixels;
		b.y = squarePerPixels;
		Gizmos::DrawLine(a, b);

		a.x = squarePerPixels;
		a.y = -squarePerPixels;
		b.x = -squarePerPixels;
		b.y = -squarePerPixels;
		Gizmos::DrawLine(a, b);

		a.x = -squarePerPixels;
		a.y = squarePerPixels;
		b.x = -squarePerPixels;
		b.y = -squarePerPixels;
		Gizmos::DrawLine(a, b);

		a.x = squarePerPixels;
		a.y = squarePerPixels;
		b.x = squarePerPixels;
		b.y = -squarePerPixels;
		Gizmos::DrawLine(a, b);
	}

	void Gizmos::DrawPosition(Vec2 origin) {
		Vec2 a = Vec2::zero, b = Vec2::zero;

		//Draw Line H 
		b = a = origin;
		a.x -= maxWorldScalar;
		b.x += maxWorldScalar;
		DrawLine(a, b);

		//Draw Line V
		b = a = origin;
		a.y -= maxWorldScalar;
		b.y += maxWorldScalar;
		DrawLine(a, b);
	}

	void Gizmos::Draw2DWorldSpace(Vec2 origin) {
		color.r = 0;
		color.g = 255;
		color.b = 0;
		color.a = 1;

		//Draw H and V position
		DrawPosition(origin);

		Vec2 dest1 = origin, dest2 = origin;
		for (size_t i = 0; i < 32; ++i)
		{
			dest1 += Vec2::one;
			DrawPosition(dest1);

			dest2 += Vec2::minusOne;
			DrawPosition(dest2);
		}
	}

	void Gizmos::DrawNavMesh(AIPathFinder::NavMesh* navMesh) {
		Vec2 lastPoint;
		Vec2 a, b;
		AIPathFinder::Neuron* p;
		register AIPathFinder::Disposition p1, p2;
		auto display = RoninApplication::instance()->display();

		color.r = 53;
		color.g = 0;
		color.b = 128;
		color.a = 255;

		navMesh->neuron(Camera::mainCamera()->ScreenToWorldPoint(Vec2(0, 0)), p1);
		navMesh->neuron(Camera::mainCamera()->ScreenToWorldPoint(Vec2(display.w, display.h)), p2);
		int x = p1.x, y = p1.y;
		while (x < p2.x)
		{
			while (y < p2.y)
			{
				p = navMesh->neuron(x, y);
				lastPoint = navMesh->PointToWorldPosition(x, y);
				color.r = !p || p->locked() ? 255 : 53;
				color.g = p && p->total() ? 100 : 0;
				
				//Draw Line H 
				b = a = lastPoint;
				a.x -= 0.03f;
				b.x += 0.03f;
				DrawLine(a, b);

				//Draw Line V
				b = a = lastPoint;
				a.y -= 0.03f;
				b.y += 0.03f;
				DrawLine(a, b);
				++y;
			}
			y = p1.y;
			++x;
		}
	}
}