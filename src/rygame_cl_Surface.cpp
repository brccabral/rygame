#include "rygame.hpp"
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
}

rg::Surface::Surface(Surface &&other) noexcept
    : Surface()
{
    *this = std::move(other);
}

rg::Surface &rg::Surface::operator=(Surface &&other) noexcept
{
    render = other.render;
    atlas_rect = other.atlas_rect;
    shared_texture = other.shared_texture;
    parent = other.parent;
    m_offset = other.m_offset;
    flip_atlas_height = other.flip_atlas_height;
    m_tint = other.m_tint;
    draw_cmds = std::move(other.draw_cmds);
    blits = std::move(other.blits);
    other.render.id = 0;
    other.render.texture.id = 0;
    other.shared_texture = nullptr;
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
                ClearBackground(color);
            });
}

void rg::Surface::SetColorKey(const rl::Color color)
{
    TraceLog(
            rl::LOG_TRACE,
            rl::TextFormat("SetColorKey render %d texture %d", render.id, render.texture.id));
    rl::Image current = LoadImageFromTextureSafe(GetTexture());
    ImageColorReplace(&current, color, rl::BLANK);
    const rl::Texture color_texture = LoadTextureFromImageSafe(current);

    // replace
    UnloadTextureSafe(render.texture);
    render.texture = color_texture;
    atlas_rect.height *= -1;

    // clean up
    UnloadImage(current);
}

void rg::Surface::SetAlpha(const float alpha)
{
    m_tint.r = alpha;
    m_tint.g = alpha;
    m_tint.b = alpha;
    m_tint.a = alpha;
}

void rg::Surface::Blit(
        Surface *incoming, const Rect &offset, const rl::BlendMode blend_mode,
        const float scale)
{
    if (!incoming)
    {
        TraceLog(rl::LOG_TRACE, "Incoming Surface is null");
        return;
    }
    Blit(incoming, offset.pos(), blend_mode, scale);
}

void rg::Surface::Blit(
        Surface *incoming, const math::Vector2<int> &offset, const rl::BlendMode blend_mode,
        const float scale)
{
    Blit(
            incoming, math::Vector2{static_cast<float>(offset.x),
                                    static_cast<float>(offset.y)}, blend_mode, scale);
}

void rg::Surface::Blit(
        Surface *incoming, const math::Vector2<float> &offset,
        const rl::BlendMode blend_mode, const float scale)
{
    Blit(
            incoming->GetTexture(), offset,
            {incoming->atlas_rect.x, incoming->atlas_rect.y, incoming->atlas_rect.width,
             incoming->atlas_rect.height * incoming->flip_atlas_height},
            blend_mode, incoming->m_tint, scale);
    blits.push_back(incoming);
}

void rg::Surface::Blit(
        const rl::Texture2D &incoming_texture, const math::Vector2<float> offset, const Rect area,
        const rl::BlendMode blend_mode, const rl::Color tint, const float scale)
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
    if (area.height && area.width)
    {
        const rl::Rectangle dest = {offset.vector2().x, offset.vector2().y,
                                    fabsf(area.width) * scale,
                                    fabsf(area.height) * scale};
        constexpr rl::Vector2 origin = {0.0f, 0.0f};

        draw_cmds.emplace_back(
                [incoming_texture, area, dest, origin, tint]
                {
                    DrawTexturePro(
                            incoming_texture, {area.x, area.y, area.width, -area.height}, dest,
                            origin, 0.0f,
                            tint);
                });
    }
    else
    {
        const rl::Rectangle dest = {offset.vector2().x, offset.vector2().y,
                                    abs(incoming_texture.width) * scale,
                                    abs(incoming_texture.height) * scale};
        constexpr rl::Vector2 origin = {0.0f, 0.0f};

        draw_cmds.emplace_back(
                [incoming_texture, area, dest, origin, tint]
                {
                    DrawTexturePro(
                            incoming_texture,
                            {area.x, area.y, (float) incoming_texture.width,
                             (float) -incoming_texture.height},
                            dest, origin, 0.0f,
                            tint);
                });
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
                    DrawTextureRec(
                            surface->GetTexture(),
                            {surface->atlas_rect.x, surface->atlas_rect.y,
                             surface->atlas_rect.width,
                             -surface->atlas_rect.height * surface->flip_atlas_height},
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
    ImageFormat(&toConvert, format);

    const rl::Texture2D converted = LoadTextureFromImageSafe(toConvert);
    UnloadTextureSafe(result.render.texture);
    result.render.texture = converted;
    result.atlas_rect.height *= -1;

    UnloadImage(toConvert);
    return result;
}

rg::Surface rg::Surface::copy() const
{
    const rl::Texture2D texture = GetTexture();
    auto result = Surface(texture.width, texture.height);
    const rl::Image toCopy = LoadImageFromTextureSafe(texture);
    const rl::Texture copyTexture = LoadTextureFromImageSafe(toCopy);
    UnloadTextureSafe(result.render.texture);
    result.render.texture = copyTexture;
    result.atlas_rect.height *= -1;
    UnloadImage(toCopy);

    return result;
}

rg::Rect rg::Surface::GetRect() const
{
    const float absWidth = atlas_rect.width > 0 ? atlas_rect.width : -atlas_rect.width;
    const float absHeight = atlas_rect.height > 0 ? atlas_rect.height : -atlas_rect.height;
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
        blit->Draw();
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
}
