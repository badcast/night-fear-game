#include "pch.h"
#include "Camera.h"
#include "Renderer.h"


RoninEngine::Runtime::Camera* _main = nullptr;
namespace RoninEngine::Runtime
{
	Camera::Camera() : Camera("Camera") {
	}
	Camera::Camera(const string& name) : Component(name) {
		if (!_main) {
			_main = this;
		}
		aspectRatio = Vec2::one;
		targetClear = true;
	}
	Camera::~Camera() {
		if (_main == this)
			_main = nullptr;
	}
	bool Camera::isFocused() {
		return _main == this;
	}
	void Camera::Focus() {
		_main = this;
	}
	std::tuple< list<Renderer*>*, list<Light*>*> Camera::GetRenderersOfScreen() {
		constexpr int Nz = 5;
		list<Renderer*> renderers[Nz];
		list<Renderer*>* target = &renderers[0];
		int zN = Nz;
		static int size = 0;

		if (__rendererOutResults.empty())
		{
            auto display = RoninApplication::display();
			Vec2 topLeft, rightBottom, rSz;
			topLeft = this->ScreenToWorldPoint(Vec2::zero);
			rightBottom = this->ScreenToWorldPoint(Vec2(display.w, display.h));

            //todo: collect
			for (auto render : Scene::getScene()->_assoc_renderers)
			{
				if (render->zOrder >= Nz)
					throw std::out_of_range("N z range");

				rSz = render->GetSize() / squarePerPixels / 2;

				Transform* t = render->transform();
				if (render->zOrder >= 0 &&
					//	X											
					(t->point.x + rSz.x >= topLeft.x && t->point.x - rSz.x <= rightBottom.x) &&
					//	Y
					(t->point.y - rSz.y <= topLeft.y && t->point.y + rSz.y >= rightBottom.y)) {
					renderers[render->zOrder].emplace_front(render);
				}
			}

			//ordering and collect
			target = &renderers[--zN];
			while (target)
			{
				for (auto i = begin(*target); i != end(*target); ++i)
				{
					__rendererOutResults.emplace_front((*i));
				}

				if (zN)
					target = &renderers[--zN];
				else
					target = NULL;
			}
		}

		if (__lightsOutResults.empty())
		{
			__lightsOutResults.assign(Scene::getScene()->_assoc_lightings.begin(), Scene::getScene()->_assoc_lightings.end());
		}

		return make_tuple(&__rendererOutResults, &__lightsOutResults);
	}
	const Vec2 Camera::ScreenToWorldPoint(Vec2 screenPoint) {
		Vec2 lhs = transform()->position();

		//dst.x = ((rect.w - dst.w) / 2.0f - (point->x + sourcePoint->x) * squarePerPixels);
		//dst.y = ((rect.h - dst.h) / 2.0f + (point->y - sourcePoint->y) * squarePerPixels);

        auto display = RoninApplication::display();
		screenPoint.x = display.w / 2.f - (screenPoint.x);
		screenPoint.x *= -1;
		screenPoint.y = display.h / 2.f - (screenPoint.y);
		screenPoint /= squarePerPixels;
		screenPoint += lhs;
		return screenPoint;
	}
	const Vec2 Camera::WorldToScreenPoint(Vec2 worldPoint) {
		Vec2* point = &transform()->point;
		Vec2 dst;
        auto display = RoninApplication::display();
		//Положение по горизонтале 
		dst.x = ((display.w ) / 2.0f - (point->x - worldPoint.x) * squarePerPixels);
		//Положение по вертикале 
		dst.y = ((display.h) / 2.0f + (point->y - worldPoint.y) * squarePerPixels);
		return dst;
	}
	Camera* Camera::mainCamera()
	{
		return _main;
	}

}
