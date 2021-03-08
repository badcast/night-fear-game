#pragma once
#include "framework.h"

namespace RoninEngine {

   namespace Runtime {
      class GameObject;
      class Object;

      // create object

      // Instancer
      template <typename T>
      class Instancer {
         public:
<<<<<<< HEAD
            Instancer() = delete;
            Instancer(Instancer&) = delete;

            static T* factory();
            static T* factory(const string& name);
            static T* factory(T* copy);
=======
            Instancer(){
               instance = allocate_class<T>(); /*
               if (RoninEngine::Scene::getScene()->main_object != nullptr &&
                   typeid(T) == typeid(GameObject)) {
                  auto root = RoninEngine::Scene::getScene()->main_object->transform();
                  Transform* tr = ((GameObject*)instance)->transform();
                  root->child_append(tr);
               }*/
            }
            Instancer(const string& name){
               instance = allocate_class<T>(name);
/*
               if (RoninEngine::Scene::getScene()->main_object != nullptr &&
                   typeid(T) == typeid(GameObject)) {
                  auto root = RoninEngine::Scene::getScene()->main_object->transform();
                  Transform* tr = ((GameObject*)instance)->transform();
                  root->child_append(tr);
               }*/
            }
            Instancer(T* val){
               instance = allocate_class<T>(*val);
               Transform* root;
/*
               if (RoninEngine::Scene::getScene()->main_object != nullptr &&
                   typeid(T) == typeid(GameObject)) {
                  root = ::RoninEngine::Scene::getCurrentScene->main_object->transform();
                  Transform* tr = ((GameObject*)instance)->transform();
                  root->child_append(tr);
               }*/
            }
            Instancer(Instancer&) = delete;

         public:
            T* instance;
>>>>>>> 500ddd76c37106f8fa32fba8b77380f136f75453
      };

      template <typename T>
      T* CreateObject() {
<<<<<<< HEAD
         return Instancer<T>::factory();
=======
         return Instancer<T>().instance;
>>>>>>> 500ddd76c37106f8fa32fba8b77380f136f75453
      }

      template <typename T>
      T* CreateObject(const string& name) {
<<<<<<< HEAD
         return 0;//return Instancer<T>::factory(name);
      }

      template <typename T>
      T* CreateObject(T* copy) {
         return 0;//Instancer<T>::factory(copy);
=======
         return Instancer<T>(name).instance;
      }

      template <typename T>
      T* CreateObject(T* ref) {
         return Instancer<T>(ref).instance;
>>>>>>> 500ddd76c37106f8fa32fba8b77380f136f75453
      }

      ///Уничтожает объект после рендера.
      void Destroy(Object* obj);

      /// Уничтожает объект после прошедшего времени.
      void Destroy(Object* obj, float t);

      ///Уничтожает объект принудительно игнорируя все условия его существования.
      void Destroy_Immediate(Object* obj);

      ///Проверка на существование объекта
      bool instanced(Object* obj);

      ///Клонирует объект
      template <typename ObjectType>
      ObjectType* Instantiate(ObjectType* obj);
      ///Клонирует объект
      GameObject* Instantiate(GameObject* obj, Vec2 position, float angle = 0);
      ///Клонирует объект
      GameObject* Instantiate(GameObject* obj, Vec2 position, Transform* parent,
                              bool worldPositionState = true);

      class Object {
            template <typename ObjectType>
            friend ObjectType* Instantiate(ObjectType* obj);
            friend GameObject* Instantiate(GameObject* obj);
            friend GameObject* Instantiate(GameObject* obj, Vec2 position, float angle);
            friend GameObject* Instantiate(GameObject* obj, Vec2 position,
                                           Transform* parent, bool worldPositionState);

            friend void Destroy(Object* obj);
            friend void Destroy(Object* obj, float t);
            friend void Destroy_Immediate(Object* obj);
            string m_name;
            std::size_t id;

         public:
            const string name() const;
            string& name();

            Object();
            Object(const string& nameobj);
            virtual ~Object() = default;

            void Destroy();

            /// Проверка на существования объекта
            operator bool();
      };

      template <typename To, typename From = Object>
      auto* Ref_to(From* from) {
         return dynamic_cast<To*>(from);
      }

      template <typename _based, typename _derived>
      constexpr bool object_base_of() {
         return std::is_base_of<_based, _derived>();
      }

      template <typename _based, typename _derived>
      constexpr bool object_base_of(_based* obj, _derived* compare) {
         return std::is_base_of<_based, _derived>();
      }
   }  // namespace Runtime
}  // namespace RoninEngine
