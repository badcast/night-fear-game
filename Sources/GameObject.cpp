#include "GameObject.h"

#include "Player.h"
#include "pch.h"

using namespace RoninEngine::Runtime;

GameObject::GameObject() : GameObject("GameObject") {}

GameObject::GameObject(const string& name) : Object(name) {
    m_components.push_back(create_empty_transform());
    m_components.front()->_derivedObject = this;
}

GameObject::~GameObject() {
    for (auto x : m_components) {
        GC::gc_unload(x);
    }
}

Transform* GameObject::transform() { return Ref_to<Transform>(m_components.front()); }

template <typename T>
T* AttribGetTypeHelper<T>::getComponent(GameObject* hier) {
    return nullptr;
}

Component* GameObject::Add_Component(Component* component) {
    if (!component) throw std::exception();

    if (end(m_components) ==
        std::find_if(begin(m_components), end(m_components), [&component](Component* ref) { return component == ref; })) {
        this->m_components.emplace_back(component);

        if (component->_derivedObject) throw bad_exception();

        component->_derivedObject = this;

        Behaviour* behav = dynamic_cast<Behaviour*>(component);
        if (behav) {
            Scene::getScene()->PinScript(behav);
            behav->OnAwake();
        } else if (Renderer* rend = dynamic_cast<Renderer*>(component)) {
            Scene::getScene()->CC_Render_Push(rend);
        } else if (Light* light = dynamic_cast<Light*>(component)) {
            Scene::getScene()->CC_Light_Push(light);
        }
    }

    return component;
}
