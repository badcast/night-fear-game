#include "Object.h"

#include "pch.h"

using namespace RoninEngine;
namespace RoninEngine {
namespace Runtime {

template <typename T>
T* factory_base(bool initInHierarchy, T* instance, const char* name) {
    if (instance == nullptr) {
        if (name == nullptr)
            GC::gc_push_lvalue<T>(instance);
        else
            GC::gc_push_lvalue<T>(instance, std::string(name));

    } else {
        instance = GC::gc_push<T>();
    }

    if (instance == nullptr) throw std::bad_alloc();

    if (initInHierarchy) {
        if (RoninEngine::Scene::getScene() == nullptr) throw std::runtime_error("var pCurrentScene is null");

        if (!RoninEngine::Scene::getScene()->is_hierarchy()) throw std::runtime_error("var pCurrentScene->mainObject is null");

        if constexpr (std::is_same<T, GameObject>::value) {
            auto root = Scene::getScene()->main_object->transform();
            Transform* tr = ((GameObject*)instance)->transform();
            root->child_append(tr);
        }
    }

    return instance;
}

Transform* create_empty_transform() { return factory_base<Transform>(false, nullptr, nullptr); }
GameObject* create_empty() { return factory_base<GameObject>(false, nullptr, nullptr); }

// NOTE: WoW: Здесь профиксина 6 месячная проблема
template GameObject* CreateObject<GameObject>();
template Transform* CreateObject<Transform>();
template Player* CreateObject<Player>();
template Camera2D* CreateObject<Camera2D>();
template Spotlight* CreateObject<Spotlight>();
template SpriteRenderer* CreateObject<SpriteRenderer>();

template GameObject* CreateObject<GameObject>(const string&);
template Transform* CreateObject<Transform>(const string&);
template Player* CreateObject<Player>(const string&);
template Camera2D* CreateObject<Camera2D>(const string&);
template Spotlight* CreateObject<Spotlight>(const string&);
template SpriteRenderer* CreateObject<SpriteRenderer>(const string&);
//-------------------------------------------------

template <typename T>
T* CreateObject() {
    return factory_base<T>(true, nullptr, nullptr);
}

template <typename T>
T* CreateObject(const string& name) {
    return factory_base<T>(true, nullptr, name.data());
}

template <typename T>
T* CreateObject(T* copy) {
    return factory_base<T>(true, copy, nullptr);
}

void Destroy(Object* obj) { Destroy(obj, 0); }

void Destroy(Object* obj, float t) {
    if (!obj || !Scene::getScene()) throw std::bad_exception();
    if (!Scene::getScene()->_destructions) {
        Scene::getScene()->_destructions = GC::gc_push<std::remove_pointer<decltype(Scene::getScene()->_destructions)>::type>();
    }

    auto ref = Scene::getScene()->_destructions;

    auto iter = std::find_if(std::begin(*ref), std::end(*ref), [obj](pair<Object*, float> x) { return obj == x.first; });

    if (iter != std::end(*ref)) std::bad_exception();

    ref->push_back(make_pair(const_cast<Object*>(obj), Time::time() + t));
}

void Destroy_Immediate(Object* obj) {
    if (!obj) throw std::runtime_error("Object is null");

    GameObject* gObj;
    if (gObj = dynamic_cast<GameObject*>(obj)) {
        if (Scene::getScene()->_firstRunScripts) {
            Scene::getScene()->_firstRunScripts->remove_if([gObj](Behaviour* x) {
                auto iter = find_if(std::begin(gObj->m_components), std::end(gObj->m_components),
                                    [x](Component* c) { return (Component*)x == c; });

                return iter != end(gObj->m_components);
            });
        } else if (Scene::getScene()->_realtimeScripts) {
            Scene::getScene()->_realtimeScripts->remove_if([gObj](Behaviour* x) {
                auto iter = find_if(std::begin(gObj->m_components), std::end(gObj->m_components),
                                    [x](Component* c) { return (Component*)x == c; });

                return iter != end(gObj->m_components);
            });
        }
    }

    // todo: деструктор для этого объекта
    SDL_Log("Object destroyed id: %d", obj->id);

    Scene::getScene()->_objects.erase(obj);
    GC::gc_unload(obj);
}

bool instanced(Object* obj) {
    if (!obj || !Scene::getScene()) throw std::bad_exception();
    auto iter = Scene::getScene()->_objects.find(obj);
    return iter != end(Scene::getScene()->_objects);
}

// Instantaite clone
template <typename T>
T* Instantiate(T* obj) {
    return CreateObject<T>();
}

GameObject* Instantiate(GameObject* obj) {
    const char _cloneStr[] = " (clone)";

    GameObject* clone = CreateObject<GameObject>();
    clone->name() = obj->name();
    if (clone->m_name.find(_cloneStr) == std::string::npos) clone->m_name += _cloneStr;

    for (auto iter = begin(obj->m_components); iter != end(obj->m_components); ++iter) {
        Component* c = *iter;
        if (dynamic_cast<Transform*>(c)) {
            Transform* t = ((Transform*)clone->m_components.front());
            // TODO: сомнительно для Transform
            *t = *dynamic_cast<Transform*>(c);
            t->_derivedObject = clone;
            t->setParent(nullptr);

            // Clone childs
            std::for_each(std::begin(reinterpret_cast<Transform*>(c)->hierarchy),
                          std::end(reinterpret_cast<Transform*>(c)->hierarchy), [t](Transform* y) {
                              // recursive
                              GameObject* yClone = Instantiate(y->gameObject());
                              y->setParent(t);
                              yClone->name() = t->gameObject()->name();  // put " (clone)" name
                              yClone->name().shrink_to_fit();
                          });
            continue;
        } else if (dynamic_cast<SpriteRenderer*>(c)) {
            c = CreateObject(reinterpret_cast<SpriteRenderer*>(c));
        } else if (false /* dynamic_cast<Camera2D*>(c)*/) {
            c = CreateObject(reinterpret_cast<Camera2D*>(c));
        } else {
            static_assert("undefined type");
            continue;
        }

        c->_derivedObject = nullptr;
        clone->Add_Component(c);
    }

    return clone;
}
GameObject* Instantiate(GameObject* obj, Vec2 position, float angle) { return nullptr; }
GameObject* Instantiate(GameObject* obj, Vec2 position, Transform* parent, bool worldPositionState) { return nullptr; }

// base class

Object::Object() : Object(typeid(Object).name()) {}

Object::Object(const string& nameobj) : m_name(nameobj) {
    m_name.shrink_to_fit();
    id = Scene::getScene()->globalID++;
    Scene::getScene()->ObjectPush(this);
}

const string Object::name() const { return m_name; }

string& Object::name() { return m_name; }

void Object::Destroy() { RoninEngine::Runtime::Destroy(this); }

Object::operator bool() { return instanced(this); }
}  // namespace Runtime
}  // namespace RoninEngine
