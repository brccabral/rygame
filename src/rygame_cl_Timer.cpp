#include "rygame.hpp"


// duration is in seconds
rg::Timer::Timer(
        const float duration, const bool repeat, const bool autostart,
        const std::function<void()> &func)
    : duration(duration), repeat(repeat), autostart(autostart), func(func)
{
    if (this->autostart)
    {
        Activate();
    }
}

void rg::Timer::Activate()
{
    active = true;
    start_time = rl::GetTime();
}

void rg::Timer::Deactivate()
{
    active = false;
    start_time = rl::GetTime();
    if (repeat)
    {
        Activate();
    }
}

void rg::Timer::Update()
{
    if (!active)
    {
        return;
    }
    const double currentTime = rl::GetTime();
    if (currentTime - start_time >= duration)
    {
        Deactivate();
        if (func)
        {
            func();
        }
    }
}
