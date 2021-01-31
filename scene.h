#pragma once
#include "framework.h"
#include "Application.h"
#include "Renderer.h"
#include "GameObject.h"
#include "Camera.h"
#include "Object.h"
#include "Behaviour.h"
#include "Light.h"

namespace RoninEngine
{
	class Scene
	{
		friend class Runtime::Object;
		friend class RoninApplication;
		friend class ui::GUI;
		friend class GameObject;
		friend class Renderer;
		friend class Camera;

		friend bool Runtime::existObject(Object* obj);
		friend void Runtime::Destroy(Object* obj);
		friend void Runtime::Destroy(Object* obj, float t);
		friend void Runtime::Destroy_Immediate(Object* obj);

	private:
		int globalID;
		bool m_isUnload;
		string m_name;
		list<Behaviour*>* _firstRunScripts;
		list<Behaviour*>* _realtimeScripts;
		list<pair<Object*, float>>* _destructions;

		list<Renderer*> _assoc_renderers;
		list<Light*> _assoc_lightings;

		std::map<Object*, float> _objects;

		void PinScript(Behaviour* obj);
		void CC_Render_Push(Renderer* rend);
		void CC_Light_Push(Light* light);
		void ObjectPush(Object* obj);

	protected:
		ui::GUI* ui;
		virtual void RenderScene(SDL_Renderer* renderer);
		virtual void RenderUI(SDL_Renderer* renderer);
		virtual void RenderSceneLate(SDL_Renderer* renderer);
	public:
		static Scene* currentScene;
		//Main or Root object
		GameObject* main_object;
		SDL_Renderer* renderer;

		Scene();
		Scene(const string& name);

		virtual ~Scene();

		string& name();

		ui::GUI* Get_GUI();
		void Unload();


		virtual void awake() = 0;
		virtual void start() = 0;
		virtual void update() = 0;
		virtual void lateUpdate() = 0;
		virtual void onDrawGizmos() = 0;
		virtual void onUnloading() = 0;
	};

}