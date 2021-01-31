#pragma once
#include "PathMoverMenu.h"

namespace RoninEngine::Runtime
{
    class PathMover : public World
    {
    public:
        PathMover()
            : World("Game Scene levels[XXX]")
        {
        }
        void on_awake() override;
        void on_start() override;
        void on_update() override;
        void on_gizmo() override;
        void on_unloading() override;
    };
}
