#include "rygame.hpp"


float rg::time::Clock::tick(const int fps)
{
    if (fps)
    {
        rl::SetTargetFPS(fps);
    }
    return rl::GetFrameTime();
}
