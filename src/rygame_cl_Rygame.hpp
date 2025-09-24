#pragma once
#include "rygame.hpp"


class Rygame
{
public:

    Rygame() = default;

    ~Rygame()
    {
        display_surface.~Surface();
        if (isSoundInit)
        {
            rl::CloseAudioDevice();
        }
        rl::CloseWindow();
    };

    rg::Surface display_surface{};
    unsigned int current_render = 0;
    bool isInit = false;
    bool isSoundInit = false;
    bool shouldQuit = false;
    std::vector<rg::mixer::Sound *> musics;
};
