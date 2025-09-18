#include "rygame.hpp"


rg::Surface
rg::transform::Flip(const Surface *surface, const bool flip_x, const bool flip_y)
{
    auto result =
            Surface(
                    (int) surface->GetRect().width, (int) surface->GetRect().height);
    result.Fill(rl::BLANK);
    result.Blit(surface->GetTexture(), {});
    if (flip_x)
    {
        result.atlas_rect.width = -result.atlas_rect.width;
    }
    if (flip_y)
    {
        result.atlas_rect.height = -result.atlas_rect.height;
    }

    return result;
}

rg::Frames rg::transform::Flip(const Frames *frames, const bool flip_x, const bool flip_y)
{
    auto result = Frames(
            frames->render.texture.width, frames->render.texture.height, frames->m_rows,
            frames->m_cols);
    result.frames = frames->frames;
    result.Fill(rl::BLANK);
    result.Blit(frames->render.texture, {});
    if (flip_x)
    {
        for (auto &frame: result.frames)
        {
            frame.width = -frame.width;
        }
    }
    if (flip_y)
    {
        for (auto &frame: result.frames)
        {
            frame.height = -frame.height;
        }
    }
    result.SetAtlas();

    return result;
}

rg::Surface rg::transform::GrayScale(const Surface *surface)
{
    auto texture = surface->GetTexture();
    rl::Image toGray = LoadImageFromTextureSafe(texture);
    ImageFormat(&toGray, rl::PIXELFORMAT_UNCOMPRESSED_GRAY_ALPHA);
    const rl::Texture2D texGray = LoadTextureFromImageSafe(toGray);
    auto result = Surface(texture.width, texture.height);
    result.Fill(rl::BLANK);
    result.Blit(texGray, {}, {});
    UnloadTextureSafe(texGray);
    UnloadImage(toGray);

    return result;
}

rg::Surface rg::transform::Scale(const Surface *surface, math::Vector2 size)
{
    const auto texture = surface->GetTexture();
    rl::Image toScale = LoadImageFromTextureSafe(texture);
    ImageResize(&toScale, (int) size.x, (int) size.y);
    const rl::Texture2D texScale = LoadTextureFromImageSafe(toScale);

    auto result = Surface((int) size.x, (int) size.y);
    result.Fill(rl::BLANK);
    result.Blit(texScale, {}, {});
    UnloadTextureSafe(texScale);
    UnloadImage(toScale);

    return result;
}

rg::Surface rg::transform::Scale2x(const Surface *surface)
{
    return Scale(
            surface, {surface->GetTexture().width * 2.0f, surface->GetTexture().height * 2.0f});
}
