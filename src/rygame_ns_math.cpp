#include "rygame.hpp"
#include "rygame_cl_Rygame.hpp"


extern Rygame *rygame;

rg::math::Vector3uc::Vector3uc(const rl::Vector3 v)
{
    x = (unsigned char) v.x;
    y = (unsigned char) v.y;
    z = (unsigned char) v.z;
}

std::uniform_real_distribution<float>
rg::math::random_uniform_dist(const float min, const float max)
{
    // ReSharper disable once CppTemplateArgumentsCanBeDeduced - it deduces to double, not float
    const std::uniform_real_distribution<float> dist(min, max);
    return dist;
}

float rg::math::random_uniform(std::uniform_real_distribution<float> dist)
{
    return dist(*rygame->gen);
}

float rg::math::get_random_uniform(const float min, const float max)
{
    if (!rygame->dists.contains(min))
    {
        rygame->dists[min] = {};
    }
    if (!rygame->dists[min].contains(max))
    {
        rygame->dists[min][max] = random_uniform_dist(min, max);
    }

    return rygame->dists[min][max](*rygame->gen);
}

float rg::math::clamp(const float value, const float min, const float max)
{
    return rl::Clamp(value, min, max);
}

bool operator!=(const rg::math::Vector3uc &lhs, const rg::math::Vector3uc &rhs)
{
    return lhs.x != rhs.x && lhs.y != rhs.y && lhs.z != rhs.z;
}

bool operator!=(const rg::math::Vector3uc &lhs, const rl::Vector3 &rhs)
{
    return lhs.x != (unsigned char) rhs.x && lhs.y != (unsigned char) rhs.y && lhs.z != (unsigned
               char) rhs.z;
}
