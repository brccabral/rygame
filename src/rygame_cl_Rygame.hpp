#pragma once
#include "rygame.hpp"


class Rygame
{
public:

    Rygame()
    {
        random_gen = new std::mt19937(rd());
    };

    ~Rygame()
    {
        delete random_gen;
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
    static std::mt19937 *random_gen;
    std::map<float, std::map<float, std::uniform_real_distribution<float>>> float_dists;
    std::map<int, std::map<int, std::uniform_int_distribution<int>>> int_dists;

};
