#include "pch.h"
#include "Component.h"
#include "GameObject.h"

namespace RoninEngine {
	namespace Runtime
	{
		//base component 
		Component::Component() : Component(typeid(Component).name()) {}

		Component::Component(const string& name) : Object(name) {
			_derivedObject = NULL;
		}

		const bool Component::isBind() {
			return _derivedObject != NULL;
		}

		GameObject* Component::gameObject() {
			return _derivedObject;
		}

		Transform* Component::transform() {
			if (!isBind())
				throw std::exception("This component isn't binding");
			return _derivedObject->Get_Component<Transform>();
		}
	}
}
