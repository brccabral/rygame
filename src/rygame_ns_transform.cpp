#include "rygame.hpp"


std::shared_ptr<rg::Surface>
rg::transform::Flip(const std::shared_ptr<Surface> &surface, const bool flip_x, const bool flip_y)
{
    const auto result =
            std::make_shared<Surface>(surface->GetRect().width, surface->GetRect().height);
    result->Fill(rl::BLANK);
    result->Blit(surface->GetTexture(), {});
    if (flip_x)
    {
        result->atlas_rect.width = -result->atlas_rect.width;
    }
    if (flip_y)
    {
        result->atlas_rect.height = -result->atlas_rect.height;
    }

    return result;
}

std::shared_ptr<rg::Frames>
rg::transform::Flip(const std::shared_ptr<Frames> &frames, const bool flip_x, const bool flip_y)
{
    const auto result = std::make_shared<Frames>(
            (float) frames->render.texture.width, (float) frames->render.texture.height,
            frames->rows, frames->cols);
    result->frames = frames->frames;
    result->Fill(rl::BLANK);
    result->Blit(frames->render.texture, {});
    if (flip_x)
    {
        for (auto &frame: result->frames)
        {
            frame.width = -frame.width;
        }
    }
    if (flip_y)
    {
        for (auto &frame: result->frames)
        {
            frame.height = -frame.height;
        }
    }
    result->SetAtlas();

    return result;
}

std::shared_ptr<rg::Surface> rg::transform::GrayScale(const std::shared_ptr<Surface> &surface)
{
    auto texture = surface->GetTexture();
    rl::Image toGray = LoadImageFromTextureSafe(texture);
    ImageFormat(&toGray, rl::PIXELFORMAT_UNCOMPRESSED_GRAY_ALPHA);
    const rl::Texture2D texGray = LoadTextureFromImageSafe(toGray);
    auto result = std::make_shared<Surface>(texture.width, texture.height);
    result->Fill(rl::BLANK);
    result->Blit(texGray, {}, {});
    UnloadTextureSafe(texGray);
    UnloadImage(toGray);

    return result;
}
