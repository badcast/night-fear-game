#include "Object.h"

#include "Camera2D.h"
#include "Component.h"
#include "SpriteRenderer.h"
#include "pch.h"

using namespace RoninEngine;

namespace RoninEngine {
   namespace Runtime {
      template <typename T>
       T* factory_base(bool initInHierarchy, T* instance = nullptr,
                      const string& name = {}) {
         using _T = std::remove_pointer_t<T>;
         if (instance == nullptr) {
            if (name.empty())
               instance = allocate_class<_T>();
            else
               instance = allocate_class<_T>(name);
         } else {
            instance = allocate_class<_T>(instance);
         }

         if (initInHierarchy) {
            if (Scene::getScene() == nullptr)
               throw std::runtime_error("var pCurrentScene is null");

            if (Scene::getScene()->main_object == nullptr)
               throw std::runtime_error("var pCurrentScene->mainObject is null");

            if (std::is_same<T, GameObject>::value) {
               auto root = Scene::getScene()->main_object->transform();
               Transform* tr = ((GameObject*)instance)->transform();
               root->child_append(tr);
            }
         }

         return instance;
      }

      template <typename T>
      T* Instancer<T>::factory() {
         return nullptr;//factory_base<T>(true);
      }
      template <typename T>
      T* Instancer<T>::factory(const string& name) {
         return factory_base<T>(true, nullptr, name);
      }
      template <typename T>
      T* Instancer<T>::factory(T* clone) {
         return factory_base<T>(true, clone);
      }

      void Destroy(Object* obj) { Destroy(obj, 0); }

      void Destroy(Object* obj, float t) {
         if (!obj || !Scene::getScene()) throw std::bad_exception();
         if (!Scene::getScene()->_destructions) {
            Scene::getScene()->_destructions =
                  allocate_class<std::remove_pointer<decltype(Scene::getScene()->_destructions)>::type>();
         }

         auto ref = Scene::getScene()->_destructions;

         auto iter =
               std::find_if(std::begin(*ref), std::end(*ref),
                            [obj](pair<Object*, float> x) { return obj == x.first; });

         if (iter != std::end(*ref)) std::bad_exception();

         ref->push_back(make_pair(const_cast<Object*>(obj), Time::time() + t));
      }

      void Destroy_Immediate(Object* obj) {
         if (!obj) throw std::runtime_error("Object is null");

         GameObject* gObj;
         if (gObj = dynamic_cast<GameObject*>(obj)) {
            if (Scene::getScene()->_firstRunScripts) {
               Scene::getScene()->_firstRunScripts->remove_if(
                        [gObj](Behaviour* x) {
                  auto iter = find_if(
                           std::begin(gObj->m_components),
                           std::end(gObj->m_components),
                           [x](Component* c) { return (Component*)x == c; });

                  return iter != end(gObj->m_components);
               });
            } else if (Scene::getScene()->_realtimeScripts) {
               Scene::getScene()->_realtimeScripts->remove_if(
                        [gObj](Behaviour* x) {
                  auto iter = find_if(
                           std::begin(gObj->m_components),
                           std::end(gObj->m_components),
                           [x](Component* c) { return (Component*)x == c; });

                  return iter != end(gObj->m_components);
               });
            }
         }

         // todo: деструктор для этого объекта
         SDL_Log("Object destroyed id: %d", obj->id);

         Scene::getScene()->_objects.erase(obj);
         free_variable(obj);
      }

      bool instanced(Object* obj) {
         if (!obj || !Scene::getScene()) throw std::bad_exception();
         auto iter = Scene::getScene()->_objects.find(obj);
         return iter != end(Scene::getScene()->_objects);
      }

      // Instantaite clone
      template <typename ObjectType>
      ObjectType* Instantiate(ObjectType* obj) {
         return CreateObject<ObjectType>();
      }

      GameObject* Instantiate(GameObject* obj) {
         const char _cloneStr[] = " (clone)";

         GameObject* clone = CreateObject<GameObject>();
         clone->name() = obj->name();
         if (clone->m_name.find(_cloneStr) == std::string::npos)
            clone->m_name += _cloneStr;

         for (auto iter = begin(obj->m_components); iter != end(obj->m_components);
              ++iter) {
            Component* c = iter.operator*();
            if (dynamic_cast<Transform*>(c)) {
               Transform* t = ((Transform*)clone->m_components.front());
               // TODO: сомнительно для Transform
               *t = *dynamic_cast<Transform*>(c);
               t->_derivedObject = clone;
               t->setParent(nullptr);

               // Clone childs
               Foreach(((Transform*)c)->hierarchy, [t](Transform* y) {
                  // recursive
                  GameObject* yClone = Instantiate(y->gameObject());
                  y->setParent(t);
                  yClone->name() =
                        t->gameObject()->name();  // put " (clone)" name
                  yClone->name().shrink_to_fit();
               });
               continue;
            } else if (dynamic_cast<SpriteRenderer*>(c)) {
               c = CreateObject<SpriteRenderer>(reinterpret_cast<SpriteRenderer*>(c));
            } else if (dynamic_cast<Camera2D*>(c)) {
               c = CreateObject<Camera2D>(dynamic_cast<Camera2D*>(c));
            }
            else
            {
               static_assert ("undefined type");
               return nullptr;
            }

            c->_derivedObject = nullptr;
            clone->Add_Component(c);
         }

         return clone;
      }
      GameObject* Instantiate(GameObject* obj, Vec2 position, float angle) {
         return nullptr;
      }
      GameObject* Instantiate(GameObject* obj, Vec2 position, Transform* parent,
                              bool worldPositionState) {
         return nullptr;
      }

      // base class

      Object::Object() : Object(typeid(Object).name()) {}

      Object::Object(const string& nameobj) : m_name(nameobj) {
         m_name.shrink_to_fit();
         id = Scene::getScene()->globalID++;
         Scene::getScene()->ObjectPush(this);
      }

      string& Object::name() { return m_name; }

      void Object::Destroy() { RoninEngine::Runtime::Destroy(this); }

      Object::operator bool() { return instanced(this); }
   }  // namespace Runtime
}  // namespace RoninEngine
