#include "rygame.hpp"
#include "rygame_cl_Rygame.hpp"


extern Rygame *rygame;

rg::math::Vector3uc::Vector3uc(const rl::Vector3 v)
{
    x = (unsigned char) v.x;
    y = (unsigned char) v.y;
    z = (unsigned char) v.z;
}

std::mt19937 *rg::math::get_rng()
{
    return rygame->random_gen;
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
    return dist(*rygame->random_gen);
}

float rg::math::get_random_uniform(const float min, const float max)
{
    if (!rygame->float_dists.contains(min))
    {
        rygame->float_dists[min] = {};
    }
    if (!rygame->float_dists[min].contains(max))
    {
        rygame->float_dists[min][max] = random_uniform_dist(min, max);
    }

    return rygame->float_dists[min][max](*rygame->random_gen);
}

float rg::math::clamp(const float value, const float min, const float max)
{
    return rl::Clamp(value, min, max);
}

std::uniform_int_distribution<int>
rg::math::random_uniform_dist(const int min, const int max)
{
    const std::uniform_int_distribution<int> dist(min, max);
    return dist;
}

int rg::math::random_uniform(std::uniform_int_distribution<int> dist)
{
    return dist(*rygame->random_gen);
}

int rg::math::get_random_uniform(const int min, const int max)
{
    if (!rygame->int_dists.contains(min))
    {
        rygame->int_dists[min] = {};
    }
    if (!rygame->int_dists[min].contains(max))
    {
        rygame->int_dists[min][max] = random_uniform_dist(min, max);
    }

    return rygame->int_dists[min][max](*rygame->random_gen);
}

int rg::math::clamp(int value, const int min, const int max)
{
    if (value < min)
    {
        value = min;
    }
    if (value > max)
    {
        value = max;
    }
    return value;
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
