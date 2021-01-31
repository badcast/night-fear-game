#ifndef _GAME_H
#define _GAME_H

#include "framework.h"
#include "ResourceManager.h"

namespace RoninEngine
{
	class RoninApplication
	{
		SDL_Renderer* renderer;
		SDL_Window* window;
		bool m_inited;
		bool m_sceneAccept;
		Scene* m_scene;
		bool m_sceneLoaded;

		void LoadGame();
		void LoadedScene();
	public:
		RoninApplication();
		~RoninApplication();

		void init();

		void LoadScene(Scene* m_scene);
		void Update_Game();
		void translate(SDL_Event* e);
		void ScreenShot(const char* filename);
		SDL_Surface* ScreenShot();
		SDL_DisplayMode display();
		void quit();
		SDL_Window* GetWindow();
		SDL_Renderer* GetRenderer();

		void back_fail(void);
		void fail(const std::string& message);
		void fail_OutOfMemory();

		static RoninApplication* instance();
	};

}
#endif