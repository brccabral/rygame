#include "rygame.hpp"


float rg::math::Vector2::magnitude() const
{
    return Vector2Length(vector2);
}

rg::math::Vector2 rg::math::Vector2::normalize() const
{
    return {Vector2Normalize(vector2)};
}

void rg::math::Vector2::normalize_ip()
{
    vector2 = Vector2Normalize(vector2);
}

float rg::math::Vector2::distance_to(const Vector2 target) const
{
    return Vector2Distance(vector2, target.vector2);
}

float rg::math::Vector2::operator[](const unsigned int &i) const
{
    if (i == 0)
    {
        return x;
    }
    if (i == 1)
    {
        return y;
    }
    throw;
}

rg::math::Vector2::operator bool() const
{
    return x || y;
}

rg::math::Vector2 operator+(const rg::math::Vector2 &lhs, const rg::math::Vector2 &rhs)
{
    return {lhs.x + rhs.x, lhs.y + rhs.y};
}

rg::math::Vector2 operator-(const rg::math::Vector2 &lhs, const rg::math::Vector2 &rhs)
{
    return {lhs.x - rhs.x, lhs.y - rhs.y};
}

rg::math::Vector2 &operator+=(rg::math::Vector2 &lhs, const rg::math::Vector2 &rhs)
{
    lhs.x += rhs.x;
    lhs.y += rhs.y;
    return lhs;
}

rg::math::Vector2 &operator-=(rg::math::Vector2 &lhs, const rg::math::Vector2 &rhs)
{
    lhs.x -= rhs.x;
    lhs.y -= rhs.y;
    return lhs;
}

rg::math::Vector2 operator*(const rg::math::Vector2 &lhs, const float scale)
{
    return {lhs.x * scale, lhs.y * scale};
}

rg::math::Vector2 &operator*=(rg::math::Vector2 &lhs, const float scale)
{
    lhs.x *= scale;
    lhs.y *= scale;
    return lhs;
}
