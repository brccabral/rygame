#include <rygame.hpp>


rg::mask::Mask
rg::mask::FromSurface(const Surface *surface, const unsigned char threshold)
{
    auto mask = Mask(surface->GetRect().width, surface->GetRect().height);
    const rl::Image surfImage = LoadImageFromTextureSafe(surface->GetTexture());
    const rl::Image alphaImage = rl::ImageFromChannel(surfImage, 3);
    const auto alphaData = (unsigned char *) alphaImage.data;
    const auto maskData = (unsigned char *) mask.image.data;
    for (int i = 0; i < mask.image.width * mask.image.height; i++)
    {
        if (alphaData[i] > threshold)
        {
            maskData[i] = 255;
        }
    }
    mask.atlas_rect = surface->atlas_rect;

    rl::UnloadImage(alphaImage);
    rl::UnloadImage(surfImage);
    return mask;
}

rg::mask::Mask
rg::mask::FromSurface(const Frames *frames, const unsigned char threshold)
{
    auto mask = Mask(frames->render.texture.width, frames->render.texture.height);
    const rl::Image surfImage = LoadImageFromTextureSafe(frames->render.texture);
    const rl::Image alphaImage = rl::ImageFromChannel(surfImage, 3);
    const auto alphaData = (unsigned char *) alphaImage.data;
    const auto maskData = (unsigned char *) mask.image.data;
    for (int i = 0; i < mask.image.width * mask.image.height; i++)
    {
        if (alphaData[i] > threshold)
        {
            maskData[i] = 255;
        }
    }
    mask.atlas_rect =
            Rect{0, 0, frames->render.texture.width, frames->render.texture.height};

    rl::UnloadImage(alphaImage);
    rl::UnloadImage(surfImage);
    return mask;
}
