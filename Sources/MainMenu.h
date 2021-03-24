#pragma once
#include "framework.h"
#include "scene.h"

namespace RoninEngine {

	class MainMenu : public Scene
	{
	public:
        MainMenu() : Scene("Main Menu") {}

		void awake();
		void start();
		void update();
		void lateUpdate();
		void onDrawGizmos();
		void onUnloading();

	};
}

