#pragma once
#include "Object.h"
#include "framework.h"

namespace RoninEngine {

   namespace Runtime {

      class Component : public Object {
         private:
            friend GameObject;
            friend GameObject* Instantiate(GameObject* obj);

            GameObject* _derivedObject;

         public:
            Component();
            explicit Component(const string& name);
            Component(const Component&) = delete;
            virtual ~Component() = default;

            const bool isBind();
            GameObject* gameObject();
            Transform* transform();
      };
   }  // namespace Runtime
}  // namespace RoninEngine
