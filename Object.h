#pragma once
#include "framework.h"
#include "scene.h"

namespace RoninEngine::Runtime
{
	class GameObject;
	class Object;

	//create object
	template<typename T>
	class Instancer {
	public:
		Instancer() {
			instance = allocate_class< T>();
			if (RoninEngine::Scene::currentScene->main_object && typeid(T) == typeid(GameObject)) {
				auto root = RoninEngine::Scene::currentScene->main_object->transform();
				Transform* tr = ((GameObject*)instance)->transform();
				root->child_append(tr);
			}
		}
		Instancer(const string& name) {
			instance = allocate_class< T>(name);
			if (RoninEngine::Scene::currentScene->main_object && typeid(T) == typeid(GameObject)) {
				auto root = RoninEngine::Scene::currentScene->main_object->transform();
				auto tr = ((GameObject*)instance)->transform();
				root->child_append(tr);
			}
		}
		Instancer(T* val) {
			instance = allocate_class< T>(*val);
			if (RoninEngine::Scene::currentScene->main_object && typeid(T) == typeid(GameObject)) {
				auto root = RoninEngine::Scene::currentScene->main_object->transform();
				auto tr = ((GameObject*)instance)->transform();
				root->child_append(tr);
			}
		}
		Instancer(Instancer&) = delete;
	public:
		T* instance;
	};

	template<typename T>
	constexpr T* CreateObject() {
		return Instancer<T>().instance;
	}

	template<typename T>
	constexpr T* CreateObject(const string& name) {
		return Instancer<T>(name).instance;
	}

	template<typename T>
	constexpr T* CreateObject(T* ref) {
		return Instancer<T>(ref).instance;
	}

	//Уничтожает объект после рендера. 
	void Destroy(Object* obj);

	// Уничтожает объект после прошедшего времени.
	void Destroy(Object* obj, float t);

	//Уничтожает объект принудительно игнорируя все условия его существования.
	void Destroy_Immediate(Object* obj);

	//Проверка на существование объекта
	bool existObject(Object* obj);

	template<typename ObjectType>
	ObjectType* Instantiate(ObjectType* obj);
	GameObject* Instantiate(GameObject* obj);
	GameObject* Instantiate(GameObject* obj, Vec2 position, float angle = 0);
	GameObject* Instantiate(GameObject* obj, Vec2 position, Transform* parent, bool worldPositionState = true);

	class Object
	{
		template<typename ObjectType>
		friend ObjectType* Instantiate(ObjectType* obj);
		friend GameObject* Instantiate(GameObject* obj);
		friend GameObject* Instantiate(GameObject* obj, Vec2 position, float angle);
		friend GameObject* Instantiate(GameObject* obj, Vec2 position, Transform* parent, bool worldPositionState);

		friend void Destroy(Object* obj);
		friend void Destroy(Object* obj, float t);
		friend void Destroy_Immediate(Object* obj);
		string m_name;
		int id;
	public:
		string& name();

		Object();
		Object(const string& nameobj);
		virtual ~Object() = default;

		void Destroy();

		// Проверка на существования объекта
		operator bool();
	};

	// "ref" as shared_ptr<Object base type> 
	template<typename To, typename From = Object>
	auto* Ref_to(From* from) {
		return dynamic_cast<To*>(from);
	}
}

