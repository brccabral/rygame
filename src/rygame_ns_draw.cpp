#include "rygame.hpp"


void rg::draw::rect(
        Surface *surface, const rl::Color color, const Rect rect,
        const float lineThick, const float radius, const bool topLeft, const bool topRight,
        const bool bottomLeft, const bool bottomRight)
{
    if (lineThick > 0)
    {
        if (radius > 0)
        {
            const float greater_dim = (rect.width < rect.height ? rect.width : rect.height) * 0.5f;
            const float r = (radius > greater_dim) ? greater_dim : radius;
            const float roundness = r / greater_dim;
            const int segments = roundness * 90;
            surface->draw_cmds.emplace_back(
                    [rect, roundness, segments, lineThick, color]
                    {
                        DrawRectangleRoundedLinesEx(
                                rect.rectangle(), roundness, segments, lineThick, color);
                    });
        }
        else
        {
            surface->draw_cmds.emplace_back(
                    [rect, lineThick, color]
                    {
                        DrawRectangleLinesEx(rect.rectangle(), lineThick, color);
                    }
                    );
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
            surface->draw_cmds.emplace_back(
                    [rect, roundness, segments, color]
                    {
                        DrawRectangleRounded(rect.rectangle(), roundness, segments, color);
                    }
                    );
            Rect corner = {0.0f, 0.0f, radius, radius};
            if (!topLeft)
            {
                corner.topleft(rect.topleft());
                surface->draw_cmds.emplace_back(
                        [corner, color]
                        {
                            DrawRectangleRec(corner.rectangle(), color);
                        }
                        );
            }
            if (!topRight)
            {
                corner.topright(rect.topright());
                surface->draw_cmds.emplace_back(
                        [corner, color]
                        {
                            DrawRectangleRec(corner.rectangle(), color);
                        });
            }
            if (!bottomLeft)
            {
                corner.bottomleft(rect.bottomleft());
                surface->draw_cmds.emplace_back(
                        [corner, color]
                        {
                            DrawRectangleRec(corner.rectangle(), color);
                        });
            }
            if (!bottomRight)
            {
                corner.bottomright(rect.bottomright());
                surface->draw_cmds.emplace_back(
                        [corner, color]
                        {
                            DrawRectangleRec(corner.rectangle(), color);
                        });
            }
        }
        else
        {
            surface->draw_cmds.emplace_back(
                    [rect, color]
                    {
                        DrawRectangleV(rect.pos().vector2(), rect.size().vector2(), color);
                    });
        }
    }
}

void rg::draw::circle(
        Surface *surface, const rl::Color color, const math::Vector2<float> center,
        const float radius, const float lineThick)
{
    if (lineThick > 0)
    {
        surface->draw_cmds.emplace_back(
                [center, radius, color]
                {
                    DrawCircleLinesV(center.vector2(), radius, color);
                });
    }
    else if (lineThick == 0)
    {
        surface->draw_cmds.emplace_back(
                [center, radius, color]
                {
                    DrawCircleV(center.vector2(), radius, color);
                });
    }
}

void rg::draw::bar(
        Surface *surface, const Rect rect, const float value,
        const float max_value, const rl::Color color, const rl::Color bg_color, const float radius)
{
    const float ratio = rect.width / max_value;
    const Rect progress_rect = {
            rect.x, rect.y, math::clamp(value * ratio, 0.0f, rect.width), rect.height};

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
        Surface *surface, const rl::Color color, const math::Vector2<float> start,
        const math::Vector2<float> end, const float width)
{
    if (width > 1)
    {
        surface->draw_cmds.emplace_back(
                [start, end, width, color]
                {
                    DrawLineEx(start.vector2(), end.vector2(), width, color);
                });
    }
    else if (width == 1)
    {
        surface->draw_cmds.emplace_back(
                [start, end, color]
                {
                    DrawLineV(start.vector2(), end.vector2(), color);
                });
    }
}

void rg::draw::lines(
        Surface *surface, const rl::Color color, const bool closed,
        const std::vector<math::Vector2<float>> &points, const float width)
{
    int pointCount = points.size();
    if (closed)
    {
        pointCount += 1;
    }

    auto *pts = new rl::Vector2[pointCount];
    for (unsigned int i = 0; i < points.size(); ++i)
    {
        pts[i] = points[i].vector2();
    }
    if (closed)
    {
        pts[pointCount - 1] = points[0].vector2();
    }

    surface->draw_cmds.emplace_back(
            [pts, pointCount, width, color]
            {
                DrawSplineLinear(pts, pointCount, width, color);
            });

    delete[] pts;
}
