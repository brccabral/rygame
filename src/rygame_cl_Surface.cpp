#include <rygame.hpp>
#include "rygame_cl_Rygame.hpp"


extern Rygame *rygame;

rg::Surface::Surface(const int width, const int height)
{
    Setup(width, height);
}

rg::Surface::Surface(const math::Vector2<float> size)
{
    Setup(size.x, size.y);
}

rg::Surface::Surface(rl::Texture2D *texture, const Rect atlas)
    : atlas_rect(atlas), shared_texture(texture)
{
    if (!atlas_rect.width)
    {
        atlas_rect.width = texture->width;
    }
    if (!atlas_rect.height)
    {
        atlas_rect.height = texture->height;
    }
    atlas_rect.height = -atlas_rect.height;
}

rg::Surface::Surface(Surface &&other) noexcept
    : Surface()
{
    *this = std::move(other);
}

rg::Surface &rg::Surface::operator=(Surface &&other) noexcept
{
    if (this != &other)
    {
        if (render.id && !parent)
        {
            UnloadRenderTextureSafe(render);
        }
        render = other.render;
        atlas_rect = other.atlas_rect;
        shared_texture = other.shared_texture;
        parent = other.parent;
        m_offset = other.m_offset;
        m_tint = other.m_tint;
        draw_cmds = std::move(other.draw_cmds);
        blits = std::move(other.blits);
        other.render.id = 0;
        other.render.texture.id = 0;
        other.shared_texture = nullptr;
    }
    return *this;
}

rg::Surface::~Surface()
{
    if (render.id && !parent)
    {
        UnloadRenderTextureSafe(render);
        render.id = 0;
        // need this trace message for Release mode pick-up the set to 0
        rl::TraceLog(rl::LOG_TRACE, "after render.id %d", render.id);
    }
}

void rg::Surface::Fill(const rl::Color color)
{
    draw_cmds.emplace_back(
            [color]
            {
                rl::ClearBackground(color);
            });
}

void rg::Surface::SetColorKey(const rl::Color color)
{
    Draw();
    rl::Image current = LoadImageFromTextureSafe(GetTexture());
    rl::ImageFormat(&current, rl::PIXELFORMAT_UNCOMPRESSED_R8G8B8A8);
    rl::ImageColorReplace(&current, color, rl::BLANK);
    auto color_texture = LoadTextureFromImageSafe(current);

    // replace
    auto tempSurf = Surface(&color_texture);
    Fill(rl::BLANK);
    Blit(&tempSurf, rg::math::Vector2<float>{});
    Draw();

    // clean up
    UnloadTextureSafe(color_texture);
    rl::UnloadImage(current);
}

void rg::Surface::SetAlpha(const float alpha)
{
    m_tint.r = alpha;
    m_tint.g = alpha;
    m_tint.b = alpha;
    m_tint.a = alpha;
}

void rg::Surface::Blit(
        Surface *incoming, const Rect &dest, const Rect &area, const rl::BlendMode blend_mode,
        const float scale_width, const float scale_height)
{
    if (!incoming)
    {
        rl::TraceLog(rl::LOG_TRACE, "Incoming Surface is null");
        return;
    }
    Blit(incoming, dest.pos(), area, blend_mode, scale_width, scale_height);
}

void rg::Surface::Blit(
        Surface *incoming, const math::Vector2<int> &dest, const Rect &area,
        const rl::BlendMode blend_mode, const float scale_width, const float scale_height)
{
    if (!incoming)
    {
        rl::TraceLog(rl::LOG_TRACE, "Incoming Surface is null");
        return;
    }
    Blit(
            incoming, math::Vector2{static_cast<float>(dest.x),
                                    static_cast<float>(dest.y)}, area, blend_mode, scale_width,
            scale_height);
}

void rg::Surface::Blit(
        Surface *incoming, const math::Vector2<float> &dest, const Rect &area,
        const rl::BlendMode blend_mode, const float scale_width, const float scale_height)
{
    if (!incoming)
    {
        rl::TraceLog(rl::LOG_TRACE, "Incoming Surface is null");
        return;
    }
    if (area.width || area.height)
    {
        Blit(
                incoming->GetTexture(), dest, area, blend_mode, incoming->m_tint, scale_width,
                scale_height);
    }
    else
    {
        Blit(
                incoming->GetTexture(), dest,
                {incoming->atlas_rect.x, incoming->atlas_rect.y, incoming->atlas_rect.width,
                 -incoming->atlas_rect.height},
                blend_mode, incoming->m_tint, scale_width, scale_height);
    }
    blits.push_back(incoming);
}

void rg::Surface::Blit(
        const rl::Texture2D &incoming_texture, const math::Vector2<float> &dest, const Rect &area,
        const rl::BlendMode blend_mode, const rl::Color tint, const float scale_width,
        const float scale_height)
{
    if (!incoming_texture.id)
    {
        return;
    }

    // draw incoming as blended
    if (blend_mode != rl::BLEND_ALPHA)
    {
        draw_cmds.emplace_back(
                [blend_mode]
                {
                    rl::BeginBlendMode(blend_mode);
                });
    }

    const float dest_width = scale_width ? scale_width : fabsf(area.width);
    const float dest_height = scale_height ? scale_height : fabsf(area.height);

    const rl::Rectangle destRect = {dest.vector2().x, dest.vector2().y, dest_width, dest_height};
    constexpr rl::Vector2 origin = {0.0f, 0.0f};

    draw_cmds.emplace_back(
            [incoming_texture, area, destRect, origin, tint]
            {
                rl::DrawTexturePro(
                        incoming_texture, area.rectangle(), destRect,
                        origin, 0.0f, tint);
            });

    if (blend_mode != rl::BLEND_ALPHA)
    {
        draw_cmds.emplace_back(
                []
                {
                    rl::EndBlendMode();
                });
    }
}

void rg::Surface::Blits(
        const std::vector<std::pair<Surface *, math::Vector2<float>>> &blit_sequence,
        const rl::BlendMode blend_mode)
{
    if (blit_sequence.empty())
    {
        return;
    }

    // draw incoming as blended
    if (blend_mode != rl::BLEND_ALPHA)
    {
        draw_cmds.emplace_back(
                [blend_mode]
                {
                    rl::BeginBlendMode(blend_mode);
                });
    }
    for (auto &[surface, offset]: blit_sequence)
    {
        draw_cmds.emplace_back(
                [surface, offset]
                {
                    rl::DrawTextureRec(
                            surface->GetTexture(),
                            {surface->atlas_rect.x, surface->atlas_rect.y,
                             surface->atlas_rect.width,
                             -surface->atlas_rect.height},
                            offset.vector2(), surface->m_tint);
                });
        blits.push_back(surface);
    }
    if (blend_mode != rl::BLEND_ALPHA)
    {
        draw_cmds.emplace_back(
                []
                {
                    rl::EndBlendMode();
                });
    }
}

rg::Surface rg::Surface::convert(const rl::PixelFormat format) const
{
    auto result = Surface(GetTexture().width, GetTexture().height);

    rl::Image toConvert = LoadImageFromTextureSafe(GetTexture());
    rl::ImageFormat(&toConvert, format);

    auto converted = LoadTextureFromImageSafe(toConvert);

    auto tempSurf = Surface(&converted);
    result.Fill(rl::BLANK);
    result.Blit(&tempSurf, rg::math::Vector2<float>{});
    result.Draw();

    UnloadTextureSafe(converted);
    rl::UnloadImage(toConvert);
    return result;
}

rg::Surface rg::Surface::copy() const
{
    const rl::Texture2D texture = GetTexture();
    auto result = Surface(texture.width, texture.height);
    const rl::Image toCopy = LoadImageFromTextureSafe(texture);
    auto copyTexture = LoadTextureFromImageSafe(toCopy);
    auto tempSurf = Surface(&copyTexture);
    result.Fill(rl::BLANK);
    result.Blit(&tempSurf, rg::math::Vector2<float>{});
    result.Draw();
    UnloadTextureSafe(copyTexture);
    rl::UnloadImage(toCopy);

    result.parent = parent;
    result.m_tint = m_tint;
    result.shared_texture = shared_texture;
    result.m_offset = m_offset;

    return result;
}

rg::Rect rg::Surface::GetRect() const
{
    const float absWidth = std::fabsf(atlas_rect.width);
    const float absHeight = std::fabsf(atlas_rect.height);
    return {0.0f, 0.0f, absWidth, absHeight};
}

rg::Surface rg::Surface::SubSurface(const Rect rect)
{
    auto result = Surface(GetTexture().width, GetTexture().height);
    UnloadRenderTextureSafe(result.render);
    result.render = render;
    result.shared_texture = shared_texture;
    result.atlas_rect = rect;
    result.parent = this;
    result.m_offset = rect.pos();
    return result;
}

rg::Surface *rg::Surface::GetParent() const
{
    return parent;
}

rg::Surface *rg::Surface::GetAbsParent()
{
    auto *result = this;
    while (result->parent)
    {
        result = result->parent;
    }
    return result;
}

rl::Texture2D rg::Surface::GetTexture() const
{
    if (shared_texture)
    {
        return *shared_texture;
    }
    return render.texture;
}

void rg::Surface::Draw()
{
    if (draw_cmds.empty())
    {
        return;
    }

    for (auto *blit: blits)
    {
        if (blit && blit != this)
        {
            blit->Draw();
        }
    }

    if (!render.id)
    {
        return;
    }

    rl::BeginTextureMode(render);
    for (auto &cmd: draw_cmds)
    {
        cmd();
    }
    rl::EndTextureMode();

    draw_cmds.clear();
    blits.clear();
}

void rg::Surface::Setup(const int width, const int height)
{
    if (!render.id)
    {
        render = LoadRenderTextureSafe(width, height);
    }

    // RenderTexture draws textures upside-down
    atlas_rect = {0, 0, width, height};

    Fill(rl::BLACK);
    Draw();
}
