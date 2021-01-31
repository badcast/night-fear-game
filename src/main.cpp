#include <cstring>

#include "PathMoverMenu.h"

using namespace std;
using namespace RoninEngine;
using namespace RoninEngine::Runtime;

// default Width screen
const int WindowWidth = 800;
// default Height screen
const int WindowHeight = 600;

int main(void)
{
#ifdef __WIN32__
    // release WINAPI console (hide console)
    FreeConsole();
#endif
    RoninSimulator::init();

    Resolution res { WindowWidth, WindowHeight };
    RoninSimulator::show(res);

    PathMoverMenu game;
    RoninSimulator::load_world(&game);

    RoninSimulator::simulate();

    return 0;
}
