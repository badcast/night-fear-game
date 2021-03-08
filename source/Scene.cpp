#include "pch.h"
#include "scene.h"

namespace RoninEngine
{
    extern Scene *pCurrentScene;
	Scene::Scene() :Scene("Untitled scene") {
	}
	Scene::Scene(const string& name)
	{
        if(pCurrentScene != nullptr){
           static_assert("pCurrentScene is and replaced by new" );
        }

		_firstRunScripts = NULL;
		_realtimeScripts = NULL;
		_destructions = NULL;
		globalID = 0;
		m_isUnload = false;
		m_name = name;

		allocate_variable(ui, this);

        //todo: Секция hierarhy требует перемены
        if(!this->is_hierarchy())
            main_object = CreateObject<GameObject>("Root Object");

        if(main_object == nullptr)
           throw std::bad_exception();
		main_object->transform()->name() = "Root Transform";
	}
	Scene::~Scene()
	{
        if (pCurrentScene == this)
        {
            static_assert("pCurrentScene set to null");
            pCurrentScene = nullptr;
        }

		if (_firstRunScripts)
		{
            free_variable(_firstRunScripts);
		}
		if (_realtimeScripts)
		{
            free_variable(_realtimeScripts);
		}
		if (_destructions)
		{
            free_variable(_destructions);
		}

		//free objects
		GameObject* target = main_object; // first
		Transform* tr;
		list<GameObject*> stack;
		while (target)
		{
			tr = target->transform();
			Foreach(tr->hierarchy, [&stack](Transform* c) {
				stack.emplace_back(c->gameObject());
			});

            free_variable(target);

			if (!stack.empty())
			{
				target = stack.front();
				stack.pop_front();
			}
			else
				target = nullptr;
		}

		this->_objects.clear();

        free_variable(ui);
	}

	void Scene::CC_Render_Push(Renderer* rend) {
		_assoc_renderers.emplace_front(rend);
	}

	void Scene::CC_Light_Push(Light* light) {
		_assoc_lightings.emplace_front(light);
	}


    void Scene::ObjectPush(Object* obj)
	{
		_objects.insert(make_pair(obj, Time::time()));
	}

	void Scene::PinScript(Behaviour* behav) {

		if (!_firstRunScripts)
			allocate_variable(_firstRunScripts);
		if (std::find_if(begin(*this->_firstRunScripts), end(*this->_firstRunScripts), [behav](Behaviour* ref) { return behav == ref; }) == end(*_firstRunScripts))
		{
			if (_realtimeScripts && std::find_if(begin(*this->_realtimeScripts), end(*this->_realtimeScripts), [behav](Behaviour* ref) { return behav == ref; }) != end(*_realtimeScripts))
				return;

			_firstRunScripts->emplace_back(behav);
		}
	}

	void Scene::RenderScene(SDL_Renderer* renderer)
	{
		if (_firstRunScripts)
		{
			if (!_realtimeScripts)
				allocate_variable(_realtimeScripts);

			Foreach(*_firstRunScripts, [this](Behaviour* b) {
				b->OnStart(); // go start (first draw)
				this->_realtimeScripts->emplace_back(b);
			});
            free_variable(_firstRunScripts);
		}
		//todo: После выполнения OnStart сразу выполняется OnUpdate исправить.
		if (_realtimeScripts)
		{
			Foreach(*_realtimeScripts, [](Behaviour* n) {
				n->OnUpdate();
            });
		}

		//Render on main camera
		Camera* cam = Camera::mainCamera(); // Рисуем сцену 
		if (cam) {
            SDL_DisplayMode display = RoninApplication::display();
			//FlushCache last result
			if (cam->targetClear)
				cam->__rendererOutResults.clear();
			// Рисуем в соотношение окна...
			cam->render(renderer, { 0, 0, display.w, display.h }, main_object);
		}

		//Destroy req objects
		if (_destructions)
		{
			if (_destructions->begin() == _destructions->end())
			{
                free_variable(_destructions);
			}
			else
			{
				_destructions->remove_if([this](pair<Object*, float>& p) {

					if (p.second <= Time::time())
					{
						Destroy_Immediate(p.first);
						return true;
					}
					return false;
				});
			}
		}

	}

	void Scene::RenderUI(SDL_Renderer* renderer) {
		//render UI
		ui->Do_Present(renderer);
	}

	void Scene::RenderSceneLate(SDL_Renderer* renderer)
	{
		if (_realtimeScripts)
		{
			Foreach(*_realtimeScripts, [](Behaviour* n) {
				n->OnLateUpdate();
			});
		}
	}
	std::string& Scene::name() {
		return this->m_name;
	}
	UI::GUI* Scene::Get_GUI() {
		return this->ui;
	}
	void Scene::Unload() {
		this->m_isUnload = true;
	}
    Scene* Scene::getScene(){
       return pCurrentScene;
    }
    bool Scene::is_hierarchy(){
      return main_object != nullptr;
    }
}
