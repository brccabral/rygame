#include <rygame.hpp>


rg::Line::Line(
        const math::Vector2<float> start, const math::Vector2<float> end) : start(start), end(end)
{
}

rg::Line::Line(
        const float x1, const float y1, const float x2, const float y2) : start(x1, y1), end(x2, y2)
{
}

bool rg::Line::collidepoint(const math::Vector2<float> point, const float threshold) const
{
    return rl::CheckCollisionPointLine(point.vector2(), start.vector2(), end.vector2(), threshold);
}

bool rg::Line::collideline(const Line other, math::Vector2<float> *collisionPoint) const
{
    rl::Vector2 collision_v2;
    const auto result = rl::CheckCollisionLines(
            start.vector2(), end.vector2(),
            other.start.vector2(), other.end.vector2(),
            &collision_v2);
    collisionPoint->x = collision_v2.x;
    collisionPoint->y = collision_v2.y;
    return result;
}

rg::Line::operator bool() const
{
    return start.x != end.x || start.y != end.x;
}
