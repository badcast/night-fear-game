#include <cstring>

#include "inputSystem.h"
#include "pch.h"
#include "sysInfo.h"

using namespace std;
using namespace RoninEngine;

void quit() {
  SDL_Quit();
  IMG_Quit();
  Mix_Quit();
}

void init_main() {
  if (SDL_Init(SDL_INIT_AUDIO | SDL_INIT_EVENTS | SDL_INIT_TIMER |
               SDL_INIT_VIDEO))
    RoninApplication::fail("Fail init main system.");
}

void init_graphics() {
  // init graphics
  if (!IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG))
    RoninApplication::fail("Fail init imageformats.");
}

void init_audio() {
  if (!Mix_Init(MIX_InitFlags::MIX_INIT_OGG | MIX_InitFlags::MIX_INIT_MP3))
    RoninApplication::fail("Fail init audio.");

  if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024))
    RoninApplication::fail("Fail open audio.");
}

void log_handler(void *userdata, int category, SDL_LogPriority priority,
                 const char *message) {}

void goGame() {

  /**/ SDL_Event e;
  /**/ SDL_DisplayMode displayMode;
  /**/ bool _queryQuit = false;
  /**/ int firstStep;
  /**/ char _title[128];
  SDL_WindowFlags wndFlags;

  SDL_LogSetOutputFunction(log_handler, NULL);

  RoninApplication::Init();
  //Загружаем главное меню
  auto scene = GC::gc_alloc_scene<GameScene>();
  RoninApplication::LoadScene(scene);

  SDL_GetDisplayMode(0, 0, &displayMode);
  while (!_queryQuit) {
    wndFlags = static_cast<SDL_WindowFlags>(
        SDL_GetWindowFlags(RoninApplication::GetWindow()));

    while (SDL_PollEvent(&e)) {
      input::Update_Input(&e);

      _queryQuit = e.type == SDL_QUIT;
    }

    if ((wndFlags & SDL_WindowFlags::SDL_WINDOW_MINIMIZED) !=
        SDL_WindowFlags::SDL_WINDOW_MINIMIZED) {

      // update
      input::Late_Update();

      RoninApplication::Update_Game();
      RoninApplication::translate(&e);

      float _ltime = 0;

      float fps = SDL_ceilf(Time::frame() / (SDL_GetTicks() / 1000.f));
      if (Time::time() > _ltime) {

#ifdef _GLIBCXX_DEBUG_ONLY
        std::sprintf(_title,
                     "NightFear (Debug) FPS:%d Memory:%luMiB, "
                     "GC_Allocated:%d, SDL_Allocated:%d",
                     static_cast<int>(fps),
                     get_process_sizeMemory() / 1024 / 1024,
                     GC::gc_total_allocated(), SDL_GetNumAllocations());

#endif
        SDL_SetWindowTitle(RoninApplication::GetWindow(), _title);
        _ltime = Time::time() + 0.15f;
      }

      Time::update();
    }
  }

  RoninApplication::Free();
}

int main(int /*argc*/, char ** /*argv[]*/) {
#ifdef __WIN32__
  FreeConsole();
#endif

  // SDL_SetMemoryFunctions(malloc, calloc, realloc, free);
  SDL_SetMemoryFunctions(&std::malloc, &std::calloc, &std::realloc, &std::free);

  init_main();
  init_graphics();
  init_audio();

  goGame();
  quit();
  return 0;
}
