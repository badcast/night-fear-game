#ifndef _GAME_H
#define _GAME_H

#include "framework.h"
#include "ResourceManager.h"

namespace RoninEngine
{
	class RoninApplication
	{
        static void LoadGame();
        static void LoadedScene();
	public:
        static void Init();
        static void Free();

        static void LoadScene(Scene* m_scene);
        static void Update_Game();
        static void translate(SDL_Event* e);
        static void ScreenShot(const char* filename);
        static SDL_Surface* ScreenShot();
        static SDL_DisplayMode display();
        static void quit();
        static SDL_Window* GetWindow();
        static SDL_Renderer* GetRenderer();

        static void back_fail(void);
        static void fail(const std::string& message);
        static void fail_OutOfMemory();
	};

}
#endif
