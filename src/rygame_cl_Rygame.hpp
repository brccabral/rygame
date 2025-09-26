#pragma once
#include "rygame.hpp"


class Rygame
{
public:

    Rygame()
    {
        gen = new std::mt19937(rd());
    };

    ~Rygame()
    {
        delete gen;
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
    std::vector<rg::mixer::Sound *> musics{};

    std::random_device rd{};
    std::mt19937 *gen;
    std::map<float, std::map<float, std::uniform_real_distribution<float>>> dists;

};
