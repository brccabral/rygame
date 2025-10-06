#include "rygame_cl_Rygame.hpp"

Rygame::Rygame()
{
    random_gen = new std::mt19937(rd());
}

Rygame::~Rygame()
{
    delete random_gen;
}
