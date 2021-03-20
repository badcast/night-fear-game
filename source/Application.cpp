#include <time.h>


#include "LevelEntity.h"
#include "inputSystem.h"
#include "pch.h"

using namespace UI;

namespace RoninEngine {
   Scene* _lastSceneToFree = nullptr;
   SDL_Renderer* renderer = nullptr;
   SDL_Window* window = nullptr;
   bool m_inited = false;
   bool m_sceneAccept = false;
   Scene* m_scene = nullptr;
   bool m_sceneLoaded = false;

   void RoninApplication::Init() {
      if (m_inited) return;

      //set locale
      setlocale(0, "ru_RU.UTF8");

      window = SDL_CreateWindow("NightFear @lightmister", SDL_WINDOWPOS_CENTERED,
                                SDL_WINDOWPOS_CENTERED, WindowWidth, WindowHeight,
                                SDL_WINDOW_SHOWN);
      renderer = SDL_CreateRenderer(
               window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

      // Brightness - Яркость
      SDL_SetWindowBrightness(window, 1);

      Time::Init_TimeEngine();

      LoadGame();

      m_inited = true;
   }

   void RoninApplication::Free() {
      GC::gc_free();
      //SDL_DestroyRenderer(renderer);
      SDL_DestroyWindow(window);
      Free_Controls();
   }

   void RoninApplication::LoadGame() {
      GC::gc_lock();

      //Загружаем данные и готовим программу к запуску

      GC::CheckResources();

      GC::gc_init();

      string path = dataAt(FolderKind::LOADER);
      string temp = path + "graphics.conf";
      GC::LoadImages(temp.c_str());

      // load textures
      path = dataAt(FolderKind::LOADER);
      temp = path + "textures.conf";
      GC::LoadImages(temp.c_str());

      //Загрузк шрифта и оптимизация дэффектов
      UI::Initialize_Fonts(true);

      //Инициализация инструментов
      InitalizeControls();

      // Set cursor
      SDL_SetCursor(GC::GetCursor("cursor", {1, 1}));

      Levels::Level_Init();
   }

   void RoninApplication::LoadedScene() {
      if (_lastSceneToFree) {
         _lastSceneToFree->onUnloading();
         if(!GC::gc_unload(_lastSceneToFree))
            static_assert (true, "Failed release ");

          _lastSceneToFree = nullptr;
         //GC::UnloadUnused();
         GC::gc_free_source();
      }

      m_sceneLoaded = true;

      //capture memory as GC
      GC::gc_unlock();

      m_scene->awake();
   }

   void RoninApplication::LoadScene(Scene* scene) {
      if (!scene || m_scene == scene) throw std::bad_cast();

      if (m_scene) {
         _lastSceneToFree = m_scene;
         m_scene->Unload();
      }

      if (!scene->is_hierarchy()) {
         // init main object
         scene->main_object = create_empty();

         if (scene->main_object == nullptr) throw std::bad_exception();

         scene->main_object->transform()->name() = "Root Transform";
      }

      m_scene = scene;
      m_sceneAccept = false;
      m_sceneLoaded = false;
   }

   SDL_Surface* RoninApplication::ScreenShot() {
      SDL_Rect rect;
      void * pixels;
      int pitch;

      SDL_RenderGetViewport(renderer, &rect);
      pitch = (rect.w - rect.x) * 4;
      pixels = malloc(pitch * (rect.h - rect.y));
      //read the Texture buffer
      SDL_RenderReadPixels(renderer, nullptr, SDL_PIXELFORMAT_RGBA8888, pixels, pitch);


      SDL_Surface *su = SDL_CreateRGBSurfaceFrom(pixels, pitch / 4, rect.h - rect.y, 32, pitch, 0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000);
      return su;
   }

   void RoninApplication::ScreenShot(const char* filename) {
      SDL_Surface* surf = ScreenShot();
      IMG_SavePNG(surf, filename);
      SDL_FreeSurface(surf);
   }

   SDL_DisplayMode RoninApplication::display() {
      SDL_DisplayMode display;
      SDL_GetWindowDisplayMode(window, &display);
      return display;
   }

   void RoninApplication::quit() {
      input::event->type = SDL_QUIT;
      SDL_PushEvent(input::event);
   }

   void RoninApplication::Update_Game() {
      SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);  // black clear
      //Очистка
      SDL_RenderClear(renderer);

      if (!m_sceneLoaded) {
         // free cache
         LoadedScene();
         m_sceneLoaded = true;
      } else {
         //Обновляем сцену
         if (!m_sceneAccept) {
            SDL_RenderFlush(renderer);  // flush renderer before first render
            m_scene->start();
            m_sceneAccept = true;
         } else {
            m_scene->update();
         }

         m_scene->RenderScene(renderer);
         m_scene->onDrawGizmos();  // Draw gizmos
         m_scene->RenderUI(renderer);

         if (!_lastSceneToFree) {
            SDL_RenderPresent(renderer);
            m_scene->lateUpdate();
            m_scene->RenderSceneLate(renderer);
         }
      }

      input::Reset();
   }

   void RoninApplication::translate(SDL_Event* e) {}

   SDL_Window* RoninApplication::GetWindow() { return window; }

   SDL_Renderer* RoninApplication::GetRenderer() { return renderer; }

   void RoninApplication::back_fail(void) { exit(EXIT_FAILURE); }

   void RoninApplication::fail(const std::string& message) {
      std::string _template = message;
      char _temp[32]{0};
      tm* ltime;
      time_t tt;
      time(&tt);

      ltime = localtime(&tt);
      strftime(_temp, sizeof(_temp), "%d.%m.%y %H:%M:%S", ltime);
      _template += "\n\n\t";
      _template += _temp;

      fprintf(stderr, _template.data());

      SDL_LogMessage(SDL_LogCategory::SDL_LOG_CATEGORY_APPLICATION,
                     SDL_LogPriority::SDL_LOG_PRIORITY_CRITICAL,
                     _template.data());
      SDL_ShowSimpleMessageBox(SDL_MessageBoxFlags::SDL_MESSAGEBOX_ERROR, "Fail",
                               _template.data(), window);
      back_fail();
   }

   void RoninApplication::fail_OutOfMemory() { fail("Out of memory!"); }
}  // namespace RoninEngine
