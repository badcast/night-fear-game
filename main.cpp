#include <cstring>

#include "sources/pch.h"
#include "sources/inputSystem.h"
#include "sources/sysInfo.h"

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
    SDL_LogMessage(SDL_LogCategory::SDL_LOG_CATEGORY_APPLICATION, SDL_LogPriority::SDL_LOG_PRIORITY_CRITICAL, message);
    SDL_ShowSimpleMessageBox(SDL_MessageBoxFlags::SDL_MESSAGEBOX_ERROR, "Fail", message, NULL);

    SDL_free(message);

    if (fail)
        exit(EXIT_FAILURE);

}

void quit()
{
    SDL_Quit();
    IMG_Quit();
    Mix_Quit();
}

void init_main()
{
    if (SDL_Init(SDL_INIT_AUDIO | SDL_INIT_EVENTS | SDL_INIT_TIMER | SDL_INIT_VIDEO))
        error("Fail init main system.");
}

void init_graphics()
{
    // init graphics
    if (!IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG))
        error("Fail init imageformats.");
}

void init_audio()
{
    if (!Mix_Init(MIX_InitFlags::MIX_INIT_OGG | MIX_InitFlags::MIX_INIT_MP3))
        error("Fail init audio.");

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024))
        error("Fail open audio.");

}

void log_handler(void* userdata, int category, SDL_LogPriority priority, const char* message) {
}

void goGame()
{
    SDL_LogSetOutputFunction(log_handler, NULL);

    SDL_Event e;
    SDL_DisplayMode displayMode;
    RoninApplication* game = allocate_class<RoninApplication>();
    SDL_GetDisplayMode(0, 0, &displayMode);

    game->init();

    bool _queryQuit = false;

    int firstStep;
    char _title[128] = {};
    strcat(_title, SDL_GetWindowTitle(game->GetWindow()));
    strcat(_title, " fps ");
    firstStep = strlen(_title);
    while (!_queryQuit)
    {
        while (SDL_PollEvent(&e))
        {
            input::Update_Input(&e);

            //?????????? ????...
            if (e.type == SDL_QUIT)
                _queryQuit = true;
        }
        //update
        input::Late_Update();

        game->Update_Game();
        game->translate(&e);

        static float _ltime = 0;
        if (!_queryQuit)
        {
            float fps = SDL_ceilf(Time::frame() / (SDL_GetTicks() / 1000.f));
            if (Time::time() > _ltime)
            {
                _title[firstStep] = NULL;// null terminator
                //show fps
                strcat(_title, to_string(fps).c_str());

                //show mem
                strcat(_title," Memory: ");
                SDL_lltoa(get_process_privateMemory() / 1024 / 1024, _title + SDL_strlen(_title), 10);
                strcat(_title,"MB/");
                SDL_lltoa(get_process_sizeMemory() / 1024 / 1024, _title + SDL_strlen(_title), 10);
                strcat(_title,"MB");

                SDL_SetWindowTitle(game->GetWindow(), _title);
                _ltime = Time::time() + 0.15f;
            }
        }
        Time::update();
    }

    deallocate_variable(game);
}

#define fg__memtest
#ifdef __memtest
typedef void* (SDLCALL* SDL_malloc_func)(size_t size);
typedef void* (SDLCALL* SDL_calloc_func)(size_t nmemb, size_t size);
typedef void* (SDLCALL* SDL_realloc_func)(void* mem, size_t size);
typedef void (SDLCALL* SDL_free_func)(void* mem);

size_t __size;
char* fullData = NULL;
size_t pos;

void* _malloc_c(size_t size)
{

    if (!fullData)
    {
        fullData = (char*)malloc(__size = 1024 * 1024 * 50);
    }
    else if (pos + size > __size)
        printf_s("Out of memory");

    void* mem = reinterpret_cast<void*>(fullData + pos + size);
    pos += size;
    return mem;
}

void* _calloc_c(size_t nmemb, size_t size)
{
    return _malloc_c(size * nmemb);//calloc(nmemb, size);
}

void* _realloc_c(void* mem, size_t size)
{
    void* _cc = _malloc_c(size);
    if (mem)
        SDL_memcpy(_cc, mem, size);

    return _cc;//realloc(mem, size);
}

void _free_c(void* mem)
{
    //free(mem);
}

#endif // __memtest

#ifdef __cplusplus
extern "C"
#endif
int main(int argc, char* argv[])
{
#ifdef __WIN32__
    FreeConsole();
#endif
#ifdef __memtest
    SDL_SetMemoryFunctions(_malloc_c, _calloc_c, _realloc_c, _free_c);
#else
    SDL_SetMemoryFunctions(malloc, calloc, realloc, free);
#endif

    init_main();
    init_graphics();
    init_audio();
    goGame();
    quit();
    return 0;
}


