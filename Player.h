#pragma once
#include "framework.h"
#include "Behaviour.h"
#include "time.h"
#include "inputSystem.h"
#include "Spotlight.h"

namespace RoninEngine::Runtime {
	class Player : public Behaviour
	{
	private:
		Camera2D *playerCamera;
		SpriteRenderer* spriteRenderer;
		Spotlight* spotlight;
	public:
		float speed = 0.05f;

		void OnAwake() override;
		void OnStart() override;
		void OnUpdate() override;
		void OnLateUpdate() override;
		void OnDestroy() override;
	
	};
}