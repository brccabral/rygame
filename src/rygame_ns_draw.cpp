#include "rygame.hpp"


void rg::draw::rect(
        const std::shared_ptr<Surface> &surface, const rl::Color color, const Rect rect,
        const float lineThick, const float radius, const bool topLeft, const bool topRight,
        const bool bottomLeft, const bool bottomRight)
{
    TraceLog(
            rl::LOG_TRACE, rl::TextFormat(
                                   "draw::rect render %d texture %d", surface->render.id,
                                   surface->render.texture.id));
    surface->ToggleRender();
    if (lineThick > 0)
    {
        if (radius > 0)
        {
            const float greater_dim = (rect.width < rect.height ? rect.width : rect.height) * 0.5f;
            const float r = (radius > greater_dim) ? greater_dim : radius;
            const float roundness = r / greater_dim;
            const int segments = roundness * 90;
            DrawRectangleRoundedLinesEx(rect.rectangle, roundness, segments, lineThick, color);
        }
        else
        {
            DrawRectangleLinesEx(rect.rectangle, lineThick, color);
        }
    }
    else if (lineThick == 0)
    {
        if (radius > 0)
        {
            const float greater_dim = (rect.width < rect.height ? rect.width : rect.height) * 0.5f;
            const float r = (radius > greater_dim) ? greater_dim : radius;
            const float roundness = r / greater_dim;
            const int segments = roundness * 90;
            DrawRectangleRounded(rect.rectangle, roundness, segments, color);
            Rect corner = {0, 0, radius, radius};
            if (!topLeft)
            {
                corner.topleft(rect.topleft());
                DrawRectangleRec(corner.rectangle, color);
            }
            if (!topRight)
            {
                corner.topright(rect.topright());
                DrawRectangleRec(corner.rectangle, color);
            }
            if (!bottomLeft)
            {
                corner.bottomleft(rect.bottomleft());
                DrawRectangleRec(corner.rectangle, color);
            }
            if (!bottomRight)
            {
                corner.bottomright(rect.bottomright());
                DrawRectangleRec(corner.rectangle, color);
            }
        }
        else
        {
            DrawRectangleV(rect.pos.vector2, rect.size.vector2, color);
        }
    }
}

void rg::draw::circle(
        const std::shared_ptr<Surface> &surface, const rl::Color color, const math::Vector2 center,
        const float radius, const float lineThick)
{
    TraceLog(
            rl::LOG_TRACE, rl::TextFormat(
                                   "draw::circle render %d texture %d", surface->render.id,
                                   surface->render.texture.id));
    surface->ToggleRender();

    if (lineThick > 0)
    {
        DrawCircleLinesV(center.vector2, radius, color);
    }
    else if (lineThick == 0)
    {
        DrawCircleV(center.vector2, radius, color);
    }
}

void rg::draw::bar(
        const std::shared_ptr<Surface> &surface, const Rect rect, const float value,
        const float max_value, const rl::Color color, const rl::Color bg_color, const float radius)
{
    TraceLog(
            rl::LOG_TRACE, rl::TextFormat(
                                   "draw::bar render %d texture %d", surface->render.id,
                                   surface->render.texture.id));
    const float ratio = rect.width / max_value;
    const Rect progress_rect = {
            rect.x, rect.y, math::clamp(value * ratio, 0, rect.width), rect.height};

    if (radius == 0)
    {
        draw::rect(surface, bg_color, rect);
        draw::rect(surface, color, progress_rect);
    }
    else
    {
        draw::rect(surface, bg_color, rect, 0, radius);
        draw::rect(surface, color, progress_rect, 0, radius);
    }
}

void rg::draw::line(
        const std::shared_ptr<Surface> &surface, const rl::Color color, const math::Vector2 start,
        const math::Vector2 end, const float width)
{
    TraceLog(
            rl::LOG_TRACE, rl::TextFormat(
                                   "draw::line render %d texture %d", surface->render.id,
                                   surface->render.texture.id));
    surface->ToggleRender();

    if (width > 1)
    {
        DrawLineEx(start.vector2, end.vector2, width, color);
    }
    else if (width == 1)
    {
        DrawLineV(start.vector2, end.vector2, color);
    }
}

void rg::draw::lines(
        const std::shared_ptr<Surface> &surface, rl::Color color, bool closed,
        std::vector<math::Vector2> points, float width)
{
    TraceLog(
            rl::LOG_TRACE, rl::TextFormat(
                                   "draw::lines render %d texture %d", surface->render.id,
                                   surface->render.texture.id));
    surface->ToggleRender();

    int pointCount = points.size();
    if (closed)
    {
        pointCount += 1;
    }

    auto *pts = new rl::Vector2[pointCount];
    for (unsigned int i = 0; i < points.size(); ++i)
    {
        pts[i] = points[i].vector2;
    }
    if (closed)
    {
        pts[pointCount - 1] = points[0].vector2;
    }

    DrawSplineLinear(pts, pointCount, width, color);

    delete[] pts;
}
