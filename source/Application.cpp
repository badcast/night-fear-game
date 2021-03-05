#include <time.h>

#include "pch.h"
#include "inputSystem.h"
#include "LevelEntity.h"

using namespace UI;

namespace RoninEngine
{
	Scene* _lastSceneToFree = NULL;
    SDL_Renderer* renderer= nullptr;
    SDL_Window* window = nullptr;
    bool m_inited= false;
    bool m_sceneAccept= false;
    Scene* m_scene = nullptr;
    bool m_sceneLoaded= false;


    void RoninApplication::Init()
	{
        if (m_inited)
            return;

        window = SDL_CreateWindow("NightFear @lightmister", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
            WindowWidth, WindowHeight, SDL_WINDOW_SHOWN);
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

        //Brightness - Яркость
        SDL_SetWindowBrightness(window, 1);

        Time::Init_TimeEngine();

        LoadGame();
        m_inited = true;
    }

    void RoninApplication::Free()
	{
        ResourceManager::ResourcesRelease();
		SDL_DestroyRenderer(renderer);
		SDL_DestroyWindow(window);
		Free_Controls();
    }

	void RoninApplication::LoadGame() {

		//Загружаем данные и готовим программу к запуску
		ResourceManager::CheckResources();

		ResourceManager::ResourcesInitialize();

		string path = dataAt(FolderKind::LOADER);
		string temp = path + "graphics";
		ResourceManager::LoadImages(temp.c_str());

		//load textures
		path = dataAt(FolderKind::TEXTURES);
		temp = path + "textures.set";
		ResourceManager::LoadImages(temp.c_str());


		//Загрузк шрифта и оптимизация дэффектов
        UI::Initialize_Fonts(true);

		//Инициализация инструментов
		InitalizeControls();

		//Set cursor
		SDL_SetCursor(ResourceManager::GetCursor("cursor", { 1, 1 }));

		//Загружаем главное меню
		LoadScene(allocate_class<GameScene>());

		Levels::Level_Init();
	}

	void RoninApplication::LoadedScene()
	{
		if (_lastSceneToFree) {
			_lastSceneToFree->onUnloading();
            free_variable(_lastSceneToFree);
			ResourceManager::UnloadUnused();
		}

		m_sceneLoaded = true;
		m_scene->awake();
	}

	void RoninApplication::LoadScene(Scene* m_scene)
	{
        if (!m_scene || m_scene == m_scene)
                        throw std::bad_cast();

        if (m_scene)
		{
            _lastSceneToFree = m_scene;
            m_scene->Unload();
		}

        m_scene = m_scene;
		m_scene->renderer = renderer;
		m_scene->ui->renderer = renderer;
		m_sceneAccept = false;
		m_sceneLoaded = false;
	}

	SDL_Surface* RoninApplication::ScreenShot()
	{
		return NULL;
	}

	void RoninApplication::ScreenShot(const char* filename)
	{
        SDL_Surface* surf = ScreenShot();
		IMG_SavePNG(surf, filename);
	}

	SDL_DisplayMode RoninApplication::display()
	{
		SDL_DisplayMode display;
		SDL_GetWindowDisplayMode(window, &display);
		return display;
	}

	void RoninApplication::quit() {
		input::event->type = SDL_QUIT;
		SDL_PushEvent(input::event);
	}

	void RoninApplication::Update_Game()
	{
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE); // black clear
		//Очистка
		SDL_RenderClear(renderer);

		if (!m_sceneLoaded)
		{
			//free cache
			LoadedScene();
			m_sceneLoaded = true;
		}
		else
		{
			//Обновляем сцену
			if (!m_sceneAccept)
			{
				SDL_RenderFlush(renderer); // flush renderer before first render
				m_scene->start();
				m_sceneAccept = true;
			}
			else
			{
				m_scene->update();
			}

			m_scene->RenderScene(renderer);
			m_scene->onDrawGizmos();//Draw gizmos
			m_scene->RenderUI(renderer);

			if (!_lastSceneToFree)
			{

				SDL_RenderPresent(renderer);
				m_scene->lateUpdate();
				m_scene->RenderSceneLate(renderer);
			}
		}

		input::Reset();
	}

	void RoninApplication::translate(SDL_Event* e)
	{
	}

	SDL_Window* RoninApplication::GetWindow()
	{
        return window;
	}

	SDL_Renderer* RoninApplication::GetRenderer()
	{
        return renderer;
	}

	void RoninApplication::back_fail(void) {
		exit(EXIT_FAILURE);
	}

	void RoninApplication::fail(const std::string& message) {
		std::string _template = message;
		char _temp[32]{ 0 };
		tm* ltime;
		time_t tt;
		time(&tt);

		ltime = localtime(&tt);
        strftime(_temp, sizeof(_temp), "%d.%m.%y %H:%M:%S", ltime);
		_template += "\n\n\t";
		_template += _temp;

		fprintf(stderr, _template.data());

		SDL_LogMessage(SDL_LogCategory::SDL_LOG_CATEGORY_APPLICATION, SDL_LogPriority::SDL_LOG_PRIORITY_CRITICAL, _template.data());
		SDL_ShowSimpleMessageBox(SDL_MessageBoxFlags::SDL_MESSAGEBOX_ERROR, "Fail", _template.data(), window);
		back_fail();

	}

	void RoninApplication::fail_OutOfMemory() {
		fail("Out of memory!");
	}
}
