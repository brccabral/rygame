#include "rygame.hpp"


bool rg::Line::collidepoint(const math::Vector2 point, const float threshold) const
{
    return CheckCollisionPointLine(point.vector2, start.vector2, end.vector2, threshold);
}

bool rg::Line::collideline(const Line other, math::Vector2 *collisionPoint) const
{
    return CheckCollisionLines(
            start.vector2, end.vector2, other.start.vector2, other.end.vector2,
            &collisionPoint->vector2);
}

rg::Line::operator bool() const
{
    return x1 != x2 || y1 != y2;
}

rg::Rect::Rect(const math::Vector2 pos, const math::Vector2 size)
    : Rect(pos.x, pos.y, size.x, size.y)
{
}

rg::Rect::Rect(rl::Rectangle rect)
    : Rect(rect.x, rect.y, rect.width, rect.height)
{
}

rg::Rect::Rect(float x, float y, float width, float height)
    : x(x), y(y), width(width), height(height)
{
}
