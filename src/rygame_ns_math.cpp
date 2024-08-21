#include "rygame.hpp"


std::random_device rd{};
std::mt19937 gen(rd());

std::uniform_real_distribution<float>
rg::math::random_uniform_dist(const float min, const float max)
{
    // ReSharper disable once CppTemplateArgumentsCanBeDeduced - it deduces to double, not float
    const std::uniform_real_distribution<float> dist(min, max);
    return dist;
}

float rg::math::random_uniform(std::uniform_real_distribution<float> dist)
{
    return dist(gen);
}

float rg::math::get_random_uniform(const float min, const float max)
{
    if (dists.find(min) == dists.end())
    {
        dists[min] = {};
    }
    if (dists[min].find(max) == dists[min].end())
    {
        dists[min][max] = random_uniform_dist(min, max);
    }

    return dists[min][max](gen);
}

float rg::math::clamp(const float value, const float min, const float max)
{
    return rl::Clamp(value, min, max);
}
