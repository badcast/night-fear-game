#pragma once
#include <ronin/framework.h>

#include "PathMover.h"

namespace RoninEngine::Runtime
{

    class PathMoverMenu : public World
    {
    public:
        PathMoverMenu()
            : World("Main Menu")
        {
        }

        void on_awake();
    };
}
