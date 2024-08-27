#pragma once
#include "rygame.hpp"


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

    rg::Surface_Ptr display_surface = nullptr;
    unsigned int current_render = 0;
    bool isSoundInit = false;
    bool shouldQuit = false;
    std::vector<rg::mixer::Sound *> musics;
};
