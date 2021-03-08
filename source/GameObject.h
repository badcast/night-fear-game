#pragma once

#include "Object.h"
#include "Transform.h"

namespace RoninEngine {
   namespace Runtime {
      //[attrib class]
      template <typename T>
      class AttribGetTypeHelper {
         public:
            static T* getType(list<Component*>& container) {
               decltype(end(container)) iter =
                     find_if(begin(container), end(container),
                             [](Component* c) { return dynamic_cast<T*>(c) != 0; });

               if (iter != end(container)) return dynamic_cast<T*>(*iter);

               return NULL;
            }

            static T* getComponent(GameObject* obj);
            static T* createEmptyComponent(GameObject *hier){
               T* component = nullptr;
               if constexpr (std::is_base_of<Component, T>::value) {
                   component = CreateObject<T>();
                   hier->Add_Component(reinterpret_cast<Component*>(component));
               } else {
                  static_assert(string("error: this type is not component: ")+ typeid(T).name());
                   throw std::bad_cast();
               }
               return component;
            }
      };

      class GameObject final : public Object {
            friend Camera2D;
            friend GameObject* Instantiate(GameObject* obj);
            friend GameObject* Instantiate(GameObject* obj, Vec2 position, float angle);
            friend GameObject* Instantiate(GameObject* obj, Vec2 position,
                                           Transform* parent, bool worldPositionState);
            friend void Destroy(Object* obj);
            friend void Destroy(Object* obj, float t);
            friend void Destroy_Immediate(Object* obj);

         private:
            list<Component*> m_components;

         public:
            GameObject();

            GameObject(const string&);
           // GameObject(const GameObject&) = delete;
            virtual ~GameObject();

            Transform* transform();

            Component* Add_Component(Component* component);

            template <typename T>
            T* Add_Component(){
               return AttribGetTypeHelper<T>::createEmptyComponent(this);
            }

            template <typename T>
            T* Get_Component() {
               return AttribGetTypeHelper<T>::getType(this->m_components);
            }
      };
   }  // namespace Runtime
}  // namespace RoninEngine
