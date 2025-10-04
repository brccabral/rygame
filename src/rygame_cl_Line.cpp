#include "rygame.hpp"


bool rg::Line::collidepoint(const math::Vector2<float> point, const float threshold) const
{
    return CheckCollisionPointLine(point.vector2(), start.vector2(), end.vector2(), threshold);
}

bool rg::Line::collideline(const Line other, math::Vector2<float> *collisionPoint) const
{
    rl::Vector2 collision_v2;
    const auto result = CheckCollisionLines(
            start.vector2(), end.vector2(), other.start.vector2(), other.end.vector2(),
            &collision_v2);
    collisionPoint->x = collision_v2.x;
    collisionPoint->y = collision_v2.y;
    return result;
}

rg::Line::operator bool() const
{
    return x1 != x2 || y1 != y2;
}

rg::Rect::Rect(const math::Vector2<float> pos, const math::Vector2<float> size)
    : Rect(pos.x, pos.y, size.x, size.y)
{
}

rg::Rect::Rect(const rl::Rectangle rect)
    : Rect(rect.x, rect.y, rect.width, rect.height)
{
}

rg::Rect::Rect(float x, float y, float width, float height)
    : x(x), y(y), width(width), height(height)
{
}
