#include <cstring>

#include "inputSystem.h"
#include "pch.h"
#include "sysInfo.h"

using namespace std;
using namespace RoninEngine;

void error(const char* errorMsg, bool fail = true) {
   char* message = reinterpret_cast<char*>(SDL_malloc(1024));
   message[0] = 0;
   strcat(message, errorMsg);
   strcat(message, "\n");
   strcat(message, "Failure (time: ");

   strcat(message, "");
   strcat(message, ")");
   SDL_LogMessage(SDL_LogCategory::SDL_LOG_CATEGORY_APPLICATION,
                  SDL_LogPriority::SDL_LOG_PRIORITY_CRITICAL, message);
   SDL_ShowSimpleMessageBox(SDL_MessageBoxFlags::SDL_MESSAGEBOX_ERROR, "Fail",
                            message, NULL);

   SDL_free(message);

   if (fail) exit(EXIT_FAILURE);
}

void quit() {
   SDL_Quit();
   IMG_Quit();
   Mix_Quit();
}

void init_main() {
   if (SDL_Init(SDL_INIT_AUDIO | SDL_INIT_EVENTS | SDL_INIT_TIMER |
                SDL_INIT_VIDEO))
      error("Fail init main system.");
}

void init_graphics() {
   // init graphics
   if (!IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG))
      error("Fail init imageformats.");
}

void init_audio() {
   if (!Mix_Init(MIX_InitFlags::MIX_INIT_OGG | MIX_InitFlags::MIX_INIT_MP3))
      error("Fail init audio.");

   if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024))
      error("Fail open audio.");
}

void log_handler(void* userdata, int category, SDL_LogPriority priority,
                 const char* message) {}

void goGame() {
   SDL_LogSetOutputFunction(log_handler, NULL);

   SDL_Event e;
   SDL_DisplayMode displayMode;
   bool _queryQuit = false;

   int firstStep;
   char _title[128] = {};
   SDL_GetDisplayMode(0, 0, &displayMode);
   RoninApplication::Init();
   strcat(_title, SDL_GetWindowTitle(RoninApplication::GetWindow()));
   strcat(_title, " fps ");
   firstStep = strlen(_title);
   while (!_queryQuit) {
      while (SDL_PollEvent(&e)) {
         input::Update_Input(&e);

         _queryQuit = e.type == SDL_QUIT;
      }
      // update
      input::Late_Update();

      RoninApplication::Update_Game();
      RoninApplication::translate(&e);

      float _ltime = 0;

      float fps = SDL_ceilf(Time::frame() / (SDL_GetTicks() / 1000.f));
      if (Time::time() > _ltime) {
         _title[firstStep] = NULL;  // null terminator
         // show fps
         strcat(_title, to_string(fps).c_str());

         // show mem
         strcat(_title, " Memory: ");
         SDL_lltoa(get_process_privateMemory() / 1024 / 1024,
                   _title + SDL_strlen(_title), 10);
         strcat(_title, "MB/");
         SDL_lltoa(get_process_sizeMemory() / 1024 / 1024,
                   _title + SDL_strlen(_title), 10);
         strcat(_title, "MB");

         SDL_SetWindowTitle(RoninApplication::GetWindow(), _title);
         _ltime = Time::time() + 0.15f;
      }

      Time::update();
   }

   RoninApplication::Free();
}

int main(int /*argc*/, char** /*argv[]*/) {
#ifdef __WIN32__
   FreeConsole();
#endif

   // SDL_SetMemoryFunctions(malloc, calloc, realloc, free);
   SDL_SetMemoryFunctions(&std::malloc, &std::calloc, &std::realloc,
                          &std::free);

   init_main();
   init_graphics();
   init_audio();

   int sizeN = std::pow(1024, 3) * 4;  // GiB
   void* ptr = SDL_malloc(sizeN);
   SDL_memset(ptr, 0xff, sizeN);
   std::memset(ptr, 0, sizeN);
   //
   SDL_free(ptr);
   goGame();
   quit();
   return 0;
}
