#pragma once
#include "rygame.hpp"

extern bool isSoundInit;

class Rygame
{
public:

    Rygame() = default;
    ~Rygame()
    {
        if (display_surface)
        {
            display_surface.reset();
            if (isSoundInit)
            {
                rl::CloseAudioDevice();
            }
            rl::CloseWindow();
        }
    };

    std::shared_ptr<rg::Surface> display_surface = nullptr;
};
