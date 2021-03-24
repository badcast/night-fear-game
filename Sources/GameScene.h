#pragma once
#include "framework.h"
#include "scene.h"

namespace RoninEngine
{
	class GameScene :
		public Scene
	{
	public:
		GameScene() : Scene("Game Scene levels[XXX]") {

		}
		void awake() override;
		void start() override;
		void update() override;
		void lateUpdate() override;
		void onDrawGizmos() override;
		void onUnloading() override;
	};
}
