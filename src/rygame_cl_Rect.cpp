#include "rygame.hpp"
#include <cassert>

/* For use with the Cohen-Sutherland algorithm for line clipping, in SDL_rect_impl.h */
#define CODE_BOTTOM 1
#define CODE_TOP 2
#define CODE_LEFT 4
#define CODE_RIGHT 8

/* Use the Cohen-Sutherland algorithm for line clipping */
static int COMPUTEOUTCODE(const rg::Rect *rect, const float x, const float y)
{
    int code = 0;
    if (y < rect->y)
    {
        code |= CODE_TOP;
    }
    else if (y >= rect->y + rect->height)
    {
        code |= CODE_BOTTOM;
    }
    if (x < rect->x)
    {
        code |= CODE_LEFT;
    }
    else if (x >= rect->x + rect->width)
    {
        code |= CODE_RIGHT;
    }
    return code;
}

float rg::Rect::right() const
{
    return x + width;
}

rg::Rect rg::Rect::right(const float v)
{
    x = v - width;
    return copy();
}

float rg::Rect::left() const
{
    return x;
}

rg::Rect rg::Rect::left(const float v)
{
    x = v;
    return copy();
}

float rg::Rect::centerx() const
{
    return x + width / 2.0f;
}

rg::Rect rg::Rect::centerx(const float v)
{
    x = v - width / 2.0f;
    return copy();
}

float rg::Rect::centery() const
{
    return y + height / 2.0f;
}

rg::Rect rg::Rect::centery(const float v)
{
    y = v - height / 2.0f;
    return copy();
}

rg::math::Vector2 rg::Rect::center() const
{
    return {x + width / 2.0f, y + height / 2.0f};
}

rg::Rect rg::Rect::center(const math::Vector2 pos)
{
    x = pos.x - width / 2.0f;
    y = pos.y - height / 2.0f;
    return copy();
}

float rg::Rect::top() const
{
    return y;
}

rg::Rect rg::Rect::top(const float v)
{
    y = v;
    return copy();
}

float rg::Rect::bottom() const
{
    return y + height;
}

rg::Rect rg::Rect::bottom(const float v)
{
    y = v - height;
    return copy();
}

rg::math::Vector2 rg::Rect::topleft() const
{
    return {x, y};
}

rg::Rect rg::Rect::topleft(const math::Vector2 pos)
{
    x = pos.x;
    y = pos.y;
    return copy();
}

rg::math::Vector2 rg::Rect::bottomleft() const
{
    return {x, y + height};
}

rg::Rect rg::Rect::bottomleft(const math::Vector2 pos)
{
    x = pos.x;
    y = pos.y - height;
    return copy();
}

rg::math::Vector2 rg::Rect::topright() const
{
    return {x + width, y};
}

rg::Rect rg::Rect::topright(const math::Vector2 pos)
{
    x = pos.x - width;
    y = pos.y;
    return copy();
}

rg::math::Vector2 rg::Rect::bottomright() const
{
    return {x + width, y + height};
}

rg::Rect rg::Rect::bottomright(const math::Vector2 pos)
{
    x = pos.x - width;
    y = pos.y - height;
    return copy();
}

rg::math::Vector2 rg::Rect::midbottom() const
{
    return {x + width / 2.0f, y + height};
}

rg::Rect rg::Rect::midbottom(const math::Vector2 pos)
{
    x = pos.x - width / 2.0f;
    y = pos.y - height;
    return copy();
}

rg::math::Vector2 rg::Rect::midtop() const
{
    return {x + width / 2.0f, y};
}

rg::Rect rg::Rect::midtop(const math::Vector2 pos)
{
    x = pos.x - width / 2.0f;
    y = pos.y;
    return copy();
}

rg::math::Vector2 rg::Rect::midleft() const
{
    return {x, y + height / 2.0f};
}

rg::Rect rg::Rect::midleft(const math::Vector2 pos)
{
    x = pos.x;
    y = pos.y - height / 2.0f;
    return copy();
}

rg::math::Vector2 rg::Rect::midright() const
{
    return {x + width, y + height / 2.0f};
}

rg::Rect rg::Rect::midright(const math::Vector2 pos)
{
    x = pos.x - width;
    y = pos.y - height / 2.0f;
    return copy();
}

rg::Rect rg::Rect::move(const math::Vector2 delta)
{
    x += delta.x;
    y += delta.y;
    return copy();
}

rg::Rect rg::Rect::inflate(const float width, const float height) const
{
    Rect result{x, y, this->width, this->height};
    result.inflate_ip(width, height);
    return result;
}

rg::Rect rg::Rect::scale_by(const float ratio) const
{
    Rect result{x, y, this->width, this->height};
    result.scale_by_ip(ratio);
    return result;
}

void rg::Rect::inflate_ip(const float width, const float height)
{
    const math::Vector2 oldCenter = center();
    this->width += width;
    this->height += height;
    center(oldCenter);
}

void rg::Rect::scale_by_ip(const float ratio)
{
    const math::Vector2 oldCenter = center();
    this->width *= ratio;
    this->height *= ratio;
    center(oldCenter);
}

rg::Rect rg::Rect::copy() const
{
    return {x, y, width, height};
}

bool rg::Rect::collidepoint(const math::Vector2 point) const
{
    return CheckCollisionPointRec(point.vector2, rectangle);
}

bool rg::Rect::collideline(
        Line line, math::Vector2 *collisionPoint1, math::Vector2 *collisionPoint2) const
{
    bool hasCollision = false;
    bool secondCollision = false;
    math::Vector2 *collisionPoint = collisionPoint1;

    const Line lineTop{topleft(), topright()};
    const Line lineBottom{bottomleft(), bottomright()};
    const Line lineLeft{topleft(), bottomleft()};
    const Line lineRight{topright(), bottomright()};

    if (line.collideline(lineTop, collisionPoint))
    {
        collisionPoint = collisionPoint2;
        hasCollision = true;
    }
    if (line.collideline(lineBottom, collisionPoint))
    {
        if (hasCollision)
        {
            secondCollision = true;
        }
        else
        {
            collisionPoint = collisionPoint2;
        }
        hasCollision = true;
    }
    if (!secondCollision && line.collideline(lineLeft, collisionPoint))
    {
        if (hasCollision)
        {
            secondCollision = true;
        }
        else
        {
            collisionPoint = collisionPoint2;
        }
        hasCollision = true;
    }
    if (!secondCollision && line.collideline(lineRight, collisionPoint))
    {
        hasCollision = true;
    }

    return hasCollision;
}

bool rg::Rect::colliderect(const Rect &other) const
{
    return CheckCollisionRecs(rectangle, other.rectangle);
}

rg::Line rg::Rect::clipline(const Line line)
{
    return clipline(line.start, line.end);
}

rg::Line rg::Rect::clipline(const math::Vector2 start, const math::Vector2 end)
{
    return clipline(start.x, start.y, end.x, end.y);
}

// SDL_INTERSECTRECTANDLINE
rg::Line rg::Rect::clipline(float x1, float y1, float x2, float y2)
{
    Line result{};

    float rectx1 = x;
    float recty1 = y;
    float rectx2 = x + width - 1;
    float recty2 = y + height - 1;
    int outcode1, outcode2;

    /* Check to see if entire line is inside rect */
    if (x1 >= rectx1 && x1 <= rectx2 && x2 >= rectx1 && x2 <= rectx2 && y1 >= recty1 &&
        y1 <= recty2 && y2 >= recty1 && y2 <= recty2)
    {
        result = {x1, y1, x2, y2};
        return result;
    }

    /* Check to see if entire line is to one side of rect */
    if ((x1 < rectx1 && x2 < rectx1) || (x1 > rectx2 && x2 > rectx2) ||
        (y1 < recty1 && y2 < recty1) || (y1 > recty2 && y2 > recty2))
    {
        return result;
    }

    if (y1 == y2)
    { /* Horizontal line, easy to clip */
        if (x1 < rectx1)
        {
            result.x1 = rectx1;
        }
        else if (x1 > rectx2)
        {
            result.x1 = rectx2;
        }
        if (x2 < rectx1)
        {
            result.x2 = rectx1;
        }
        else if (x2 > rectx2)
        {
            result.x2 = rectx2;
        }
        return result;
    }

    if (x1 == x2)
    { /* Vertical line, easy to clip */
        if (y1 < recty1)
        {
            result.y1 = recty1;
        }
        else if (y1 > recty2)
        {
            result.y1 = recty2;
        }
        if (y2 < recty1)
        {
            result.y2 = recty1;
        }
        else if (y2 > recty2)
        {
            result.y2 = recty2;
        }
        return result;
    }

    /* More complicated Cohen-Sutherland algorithm */
    outcode1 = COMPUTEOUTCODE(this, x1, y1);
    outcode2 = COMPUTEOUTCODE(this, x2, y2);
    while (outcode1 || outcode2)
    {
        if (outcode1 & outcode2)
        {
            return result;
        }

        if (outcode1)
        {
            if (outcode1 & CODE_TOP)
            {
                y = recty1;
                x = (x1 + ((x2 - x1) * (y - y1)) / (y2 - y1));
            }
            else if (outcode1 & CODE_BOTTOM)
            {
                y = recty2;
                x = (x1 + ((x2 - x1) * (y - y1)) / (y2 - y1));
            }
            else if (outcode1 & CODE_LEFT)
            {
                x = rectx1;
                y = (y1 + ((y2 - y1) * (x - x1)) / (x2 - x1));
            }
            else if (outcode1 & CODE_RIGHT)
            {
                x = rectx2;
                y = (y1 + ((y2 - y1) * (x - x1)) / (x2 - x1));
            }
            x1 = x;
            y1 = y;
            outcode1 = COMPUTEOUTCODE(this, x, y);
        }
        else
        {
            if (outcode2 & CODE_TOP)
            {
                assert(y2 != y1); /* if equal: division by zero. */
                y = recty1;
                x = x1 + (x2 - x1) * (y - y1) / (y2 - y1);
            }
            else if (outcode2 & CODE_BOTTOM)
            {
                assert(y2 != y1); /* if equal: division by zero. */
                y = recty2;
                x = x1 + (x2 - x1) * (y - y1) / (y2 - y1);
            }
            else if (outcode2 & CODE_LEFT)
            {
                /* If this assertion ever fires, here's the static analysis that warned about it:
                   http://buildbot.libsdl.org/sdl-static-analysis/sdl-macosx-static-analysis/sdl-macosx-static-analysis-1101/report-b0d01a.html#EndPath
                 */
                assert(x2 != x1); /* if equal: division by zero. */
                x = rectx1;
                y = y1 + (y2 - y1) * (x - x1) / (x2 - x1);
            }
            else if (outcode2 & CODE_RIGHT)
            {
                /* If this assertion ever fires, here's the static analysis that warned about it:
                   http://buildbot.libsdl.org/sdl-static-analysis/sdl-macosx-static-analysis/sdl-macosx-static-analysis-1101/report-39b114.html#EndPath
                 */
                assert(x2 != x1); /* if equal: division by zero. */
                x = rectx2;
                y = y1 + (y2 - y1) * (x - x1) / (x2 - x1);
            }
            x2 = x;
            y2 = y;
            outcode2 = COMPUTEOUTCODE(this, x, y);
        }
    }
    result.x1 = x1;
    result.y1 = y1;
    result.x2 = x2;
    result.y2 = y2;

    return result;
}
