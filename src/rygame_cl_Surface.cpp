#include "rygame.hpp"


rg::Surface::Surface(const int width, const int height)
{
    Setup(width, height);
}

rg::Surface::Surface(const math::Vector2 size)
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

rg::Surface::~Surface()
{
    if (render.id && !parent)
    {
        UnloadRenderTextureSafe(render);
        render.id = 0;
    }
}

void rg::Surface::Fill(const rl::Color color)
{
    TraceLog(
            rl::LOG_TRACE,
            rl::TextFormat("Fill render %d texture %d", render.id, render.texture.id));
    ToggleRender();
    ClearBackground(color);
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
    Fill(rl::BLANK);
    Blit(color_texture, {}, atlas_rect);

    // clean up
    UnloadTextureSafe(color_texture);
    UnloadImage(current);
}

void rg::Surface::SetAlpha(const float alpha)
{
    tint.r = alpha;
    tint.g = alpha;
    tint.b = alpha;
    tint.a = alpha;
}

void rg::Surface::Blit(
        const std::shared_ptr<Surface> &incoming, const Rect offset, const rl::BlendMode blend_mode)
{
    Blit(incoming, offset.pos, blend_mode);
}

void rg::Surface::Blit(
        const std::shared_ptr<Surface> &incoming, const math::Vector2 offset,
        const rl::BlendMode blend_mode)
{
    TraceLog(
            rl::LOG_TRACE, "Blit render %d texture %d Texture() %d into render %d texture %d",
            incoming->render.id, incoming->render.texture.id, incoming->GetTexture().id, render.id,
            render.texture.id);
    this->Blit(
            incoming->GetTexture(), offset,
            {incoming->atlas_rect.x, incoming->atlas_rect.y, incoming->atlas_rect.width,
             incoming->atlas_rect.height * incoming->flip_atlas_height},
            blend_mode, incoming->tint);
}

void rg::Surface::Blit(
        const rl::Texture2D &incoming_texture, const math::Vector2 offset, const Rect area,
        const rl::BlendMode blend_mode, const rl::Color tint)
{
    if (!incoming_texture.id)
    {
        return;
    }
    TraceLog(
            rl::LOG_TRACE, "Blit texture %d into render %d texture %d", incoming_texture.id,
            render.id, render.texture.id);

    ToggleRender();

    // draw incoming as blended
    if (blend_mode != rl::BLEND_ALPHA)
    {
        BeginBlendMode(blend_mode);
    }
    if (area.height && area.width)
    {
        DrawTextureRec(
                incoming_texture, {area.x, area.y, area.width, -area.height}, offset.vector2, tint);
    }
    else
    {
        DrawTextureRec(
                incoming_texture,
                {0, 0, (float) incoming_texture.width, (float) -incoming_texture.height},
                offset.vector2, tint);
    }
    if (blend_mode != rl::BLEND_ALPHA)
    {
        rl::EndBlendMode();
    }
}

void rg::Surface::Blits(
        const std::vector<std::pair<std::shared_ptr<Surface>, math::Vector2>> &blit_sequence,
        const rl::BlendMode blend_mode)
{
    if (blit_sequence.empty())
    {
        return;
    }
    TraceLog(rl::LOG_DEBUG, rl::TextFormat("Blits %d sequences", blit_sequence.size()));

    ToggleRender();

    // draw incoming as blended
    if (blend_mode != rl::BLEND_ALPHA)
    {
        BeginBlendMode(blend_mode);
    }
    for (auto &[surface, offset]: blit_sequence)
    {
        DrawTextureRec(
                surface->GetTexture(),
                {surface->atlas_rect.x, surface->atlas_rect.y, surface->atlas_rect.width,
                 -surface->atlas_rect.height * surface->flip_atlas_height},
                offset.vector2, surface->tint);
    }
    if (blend_mode != rl::BLEND_ALPHA)
    {
        rl::EndBlendMode();
    }
}

std::shared_ptr<rg::Surface> rg::Surface::convert(const rl::PixelFormat format) const
{
    const auto result = std::make_shared<Surface>(GetTexture().width, GetTexture().height);

    rl::Image toConvert = LoadImageFromTextureSafe(GetTexture());
    ImageFormat(&toConvert, format);

    const rl::Texture2D converted = LoadTextureFromImageSafe(toConvert);
    result->Blit(converted, {}, {});

    UnloadTextureSafe(converted);
    UnloadImage(toConvert);
    return result;
}

std::shared_ptr<rg::Surface> rg::Surface::copy() const
{
    rl::Texture2D texture = GetTexture();
    auto result = std::make_shared<Surface>(texture.width, texture.height);
    const rl::Image toCopy = LoadImageFromTextureSafe(texture);
    const rl::Texture copyTexture = LoadTextureFromImageSafe(toCopy);
    result->Blit(copyTexture, {}, {});
    UnloadTextureSafe(texture);
    UnloadImage(toCopy);

    return result;
}

rg::Rect rg::Surface::GetRect() const
{
    const float absWidth = atlas_rect.width > 0 ? atlas_rect.width : -atlas_rect.width;
    const float absHeight = atlas_rect.height > 0 ? atlas_rect.height : -atlas_rect.height;
    return {0, 0, absWidth, absHeight};
}

std::shared_ptr<rg::Surface> rg::Surface::SubSurface(const Rect rect)
{
    auto result = std::make_shared<Surface>(GetTexture().width, GetTexture().height);
    UnloadRenderTextureSafe(result->render);
    result->render = render;
    result->shared_texture = shared_texture;
    result->atlas_rect = rect;
    result->parent = shared_from_this();
    result->offset = rect.pos;
    return result;
}

std::shared_ptr<rg::Surface> rg::Surface::GetParent()
{
    return parent;
}

std::shared_ptr<rg::Surface> rg::Surface::GetAbsParent()
{
    std::shared_ptr<Surface> result = shared_from_this();
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

void rg::Surface::ToggleRender()
{
    if (current_render != render.id)
    {
        EndTextureModeSafe();
        TraceLog(
                rl::LOG_TRACE,
                rl::TextFormat("Begin render %d texture %d", render.id, render.texture.id));
        BeginTextureModeSafe(render);
        shared_texture = nullptr;
    }
}

void rg::Surface::Setup(const int width, const int height)
{
    if (!render.id)
    {
        render = LoadRenderTextureSafe(width, height);
    }

    // RenderTexture draws textures upside-down
    atlas_rect = {0, 0, (float) width, (float) height};

    Fill(rl::BLACK);
}
