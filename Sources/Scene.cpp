#include "pch.h"

namespace RoninEngine {
Scene *pCurrentScene;

Scene::Scene() : Scene("Untitled scene") {}
Scene::Scene(const string &name) {
    if (pCurrentScene != nullptr) {
        static_assert("pCurrentScene is and replaced by new");
    }
    pCurrentScene = this;
    _firstRunScripts = nullptr;
    _realtimeScripts = nullptr;
    _destructions = nullptr;
    globalID = 0;
    m_isUnload = false;
    m_name = name;

    GC::gc_alloc_lval(ui, this);
}
Scene::~Scene() {
    GameObject *target = main_object;  // first
    Transform *tr;
    list<GameObject *> stack;

    if (pCurrentScene == this) {
        static_assert("pCurrentScene set to null");
        pCurrentScene = nullptr;
    }

    if (_firstRunScripts) {
        GC::gc_unalloc(_firstRunScripts);
    }
    if (_realtimeScripts) {
        GC::gc_unalloc(_realtimeScripts);
    }
    if (_destructions) {
        GC::gc_unalloc(_destructions);
    }

    // free objects
    while (target) {
        tr = target->transform();
        for (auto c : tr->hierarchy) {
            stack.emplace_back(c->gameObject());
        }

        GC::gc_unalloc(target);

        if (!stack.empty()) {
            target = stack.front();
            stack.pop_front();
        } else
            target = nullptr;
    }

    this->_objects.clear();

    GC::gc_unalloc(ui);
}

void Scene::CC_Render_Push(Renderer *rend) { _assoc_renderers.emplace_front(rend); }

void Scene::CC_Light_Push(Light *light) { _assoc_lightings.emplace_front(light); }

void Scene::ObjectPush(Object *obj) { _objects.insert(make_pair(obj, Time::time())); }

void Scene::PinScript(Behaviour *behav) {
    if (!_firstRunScripts) GC::gc_alloc_lval(_firstRunScripts);
    if (std::find_if(begin(*this->_firstRunScripts), end(*this->_firstRunScripts),
                     [behav](Behaviour *ref) { return behav == ref; }) == end(*_firstRunScripts)) {
        if (_realtimeScripts && std::find_if(begin(*this->_realtimeScripts), end(*this->_realtimeScripts),
                                             [behav](Behaviour *ref) { return behav == ref; }) != end(*_realtimeScripts))
            return;

        _firstRunScripts->emplace_back(behav);
    }
}

void Scene::RenderScene(SDL_Renderer *renderer) {
    if (_firstRunScripts) {
        if (!_realtimeScripts) {
            GC::gc_alloc_lval(_realtimeScripts);
            if (_realtimeScripts == nullptr) throw std::bad_alloc();
        }

        for (auto x : *_firstRunScripts) {
            x->OnStart();  // go start (first draw)
            this->_realtimeScripts->emplace_back(x);
        }
        GC::gc_unalloc(_firstRunScripts);
        _firstRunScripts = nullptr;
    }
    // todo: После выполнения OnStart сразу выполняется OnUpdate исправить.
    if (_realtimeScripts) {
        for (auto n : *_realtimeScripts) {
            n->OnUpdate();
        };
    }

    // Render on main camera
    Camera *cam = Camera::mainCamera();  // Рисуем сцену
    if (cam) {
        SDL_DisplayMode display = Application::display();
        // FlushCache last result
        if (cam->targetClear) cam->__rendererOutResults.clear();
        // Рисуем в соотношение окна...
        cam->render(renderer, {0, 0, display.w, display.h}, main_object);
    }

    // Destroy req objects
    if (_destructions) {
        if (_destructions->begin() == _destructions->end()) {
            GC::gc_unalloc(_destructions);
        } else {
            _destructions->remove_if([](pair<Object *, float> &p) {
                if (p.second <= Time::time()) {
                    Destroy_Immediate(p.first);
                    return true;
                }
                return false;
            });
        }
    }
}

void Scene::RenderUI(SDL_Renderer *renderer) {
    // render UI
    ui->Do_Present(renderer);
}

void Scene::RenderSceneLate(SDL_Renderer *renderer) {
    if (_realtimeScripts) {
        for (auto n : *_realtimeScripts) {
            n->OnLateUpdate();
        }
    }
}
bool Scene::is_hierarchy() { return this->main_object != nullptr; }
std::string &Scene::name() { return this->m_name; }
UI::GUI *Scene::Get_GUI() { return this->ui; }
void Scene::Unload() { this->m_isUnload = true; }
Scene *Scene::getScene() { return pCurrentScene; }
}  // namespace RoninEngine
