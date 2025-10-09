#include <rygame.hpp>


rg::Surface
rg::transform::Flip(Surface *surface, const bool flip_x, const bool flip_y)
{
    auto result =
            Surface(
                    (int) surface->GetRect().width, (int) surface->GetRect().height);
    result.Fill(rl::BLANK);
    result.Blit(surface, rg::math::Vector2<float>{});
    result.Draw();
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

rg::Frames rg::transform::Flip(Frames *frames, const bool flip_x, const bool flip_y)
{
    auto result = Frames(
            frames->GetTexture().width, frames->GetTexture().height, frames->m_rows,
            frames->m_cols);
    result.frames = frames->frames;
    result.Fill(rl::BLANK);
    result.Blit(
            frames, rg::math::Vector2<float>{},
            {0, 0, frames->GetTexture().width, -frames->GetTexture().height});
    result.Draw();
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
    const auto texture = surface->GetTexture();
    rl::Image toGray = LoadImageFromTextureSafe(texture);
    rl::ImageFormat(&toGray, rl::PIXELFORMAT_UNCOMPRESSED_GRAY_ALPHA);
    const rl::Texture2D texGray = LoadTextureFromImageSafe(toGray);
    auto result = Surface(texture.width, texture.height);
    result.ApplyTexture(texGray);
    UnloadTextureSafe(texGray);
    rl::UnloadImage(toGray);

    return result;
}

rg::Surface rg::transform::Scale(const Surface *surface, const math::Vector2<float> size)
{
    const auto texture = surface->GetTexture();
    rl::Image toScale = LoadImageFromTextureSafe(texture);
    rl::ImageResize(&toScale, (int) size.x, (int) size.y);
    const rl::Texture2D texScale = LoadTextureFromImageSafe(toScale);

    auto result = Surface((int) size.x, (int) size.y);
    result.ApplyTexture(texScale);
    UnloadTextureSafe(texScale);
    rl::UnloadImage(toScale);

    return result;
}

rg::Surface rg::transform::Scale2x(const Surface *surface)
{
    return Scale(
            surface, {surface->GetTexture().width * 2.0f, surface->GetTexture().height * 2.0f});
}
