#include "pch.h"
#include "Object.h"
#include "Component.h"
#include "SpriteRenderer.h"
#include "Camera2D.h"

namespace RoninEngine::Runtime {

	void Destroy(Object* obj) {
		Destroy(obj, 0);
	}

	void Destroy(Object* obj, float t) {
		if (!obj || !Scene::currentScene)
			throw std::bad_exception();
		if (!Scene::currentScene->_destructions)
		{
			Scene::currentScene->_destructions = allocate_class<remove_pointer<decltype(Scene::currentScene->_destructions)>::type>();
		}

		auto ref = Scene::currentScene->_destructions;

		auto iter = std::find_if(std::begin(*ref), std::end(*ref), [obj](pair<Object*, float> x) {
			return obj == x.first;
		});

		if (iter != std::end(*ref))
			std::bad_exception();

		ref->push_back(make_pair(const_cast<Object*>(obj), Time::time() + t));
	}

	void Destroy_Immediate(Object* obj) {
		if (!obj)
			throw std::runtime_error("Object is null");

		GameObject* gObj;
		if (gObj = dynamic_cast<GameObject*>(obj))
		{
			if (Scene::currentScene->_firstRunScripts) {
                Scene::currentScene->_firstRunScripts->remove_if([gObj](Behaviour* x) {

					auto iter = find_if(std::begin(gObj->m_components), std::end(gObj->m_components), [x](Component* c) {
						return (Component*)x == c;
					});

					return iter != end(gObj->m_components);
				});
			}
			else if (Scene::currentScene->_realtimeScripts)
			{
				Scene::currentScene->_realtimeScripts->remove_if([gObj](Behaviour* x) {
					auto iter = find_if(std::begin(gObj->m_components), std::end(gObj->m_components), [x](Component* c) {
						return (Component*)x == c;
					});

					return iter != end(gObj->m_components);
				});
			}
		}

		//todo: деструктор для этого объекта
		SDL_Log("Object destroyed id: %d", obj->id);

		Scene::currentScene->_objects.erase(obj);
        free_variable(obj);
	}

	bool existObject(Object* obj) {

		if (!obj || !Scene::currentScene)
			throw std::bad_exception();
		auto iter = Scene::currentScene->_objects.find(obj);
		return iter != end(Scene::currentScene->_objects);
	}

	//Instantaite clone
	template<typename ObjectType>
	ObjectType* Instantiate(ObjectType* obj) {
        throw std::bad_cast();
        //ObjectType* CreateObject<ObjectType>();
		return NULL;
	}

	GameObject* Instantiate(GameObject* obj) {
		const char _cloneStr[] = " (clone)";

		GameObject* clone = CreateObject<GameObject>();
		clone->name() = obj->name();
		if (clone->m_name.find(_cloneStr)==std::string::npos)
			clone->m_name += _cloneStr;
		
		for (auto iter = begin(obj->m_components); iter != end(obj->m_components); ++iter)
		{
			Component* c = iter.operator*();
			if (dynamic_cast<Transform*>(c)){
				Transform* t = ((Transform*)clone->m_components.front());
				//TODO: сомнительно для Transform
				*t = *dynamic_cast<Transform*>(c);
				t->_derivedObject = clone;
				t->setParent(NULL);

				//Clone childs
				Foreach(((Transform*)c)->hierarchy, [t](Transform* y) {
					//recursive
					GameObject* yClone = Instantiate(y->gameObject());
					y->setParent(t);
					yClone->name() = t->gameObject()->name(); // put " (clone)" name
					yClone->name().shrink_to_fit();
				});
				continue;
			}
			else if (dynamic_cast<SpriteRenderer*>(c)) {
				c = CreateObject<SpriteRenderer>((SpriteRenderer*)c);
			}
			else if (dynamic_cast<Camera2D*>(c)) {
				c = CreateObject<Camera2D>(dynamic_cast<Camera2D*>(c));
			}

			c->_derivedObject = NULL;
			clone->Add_Component(c);
		}

		return clone;
	}
	GameObject* Instantiate(GameObject* obj, Vec2 position, float angle) {
		return NULL;
	}
	GameObject* Instantiate(GameObject* obj, Vec2 position, Transform* parent, bool worldPositionState) {
		return NULL;
	}

	//base class

	Object::Object() : Object(typeid(Object).name())
	{}

	Object::Object(const string& nameobj) : m_name(nameobj){
		m_name.shrink_to_fit();
		id = Scene::currentScene->globalID++;
		Scene::currentScene->ObjectPush(this);
	}

	string& Object::name()
	{
		return m_name;
	}

	void Object::Destroy()
	{
		RoninEngine::Runtime::Destroy(this);
	}

	Object::operator bool() {
		return existObject(this);
	}
}
