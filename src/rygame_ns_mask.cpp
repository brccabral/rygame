#include "rygame.hpp"


rg::mask::Mask
rg::mask::FromSurface(const Surface_Ptr &surface, const unsigned char threshold)
{
    auto mask = Mask(surface->GetRect().width, surface->GetRect().height);
    const rl::Image surfImage = LoadImageFromTextureSafe(surface->GetTexture());
    const rl::Image alphaImage = ImageFromChannel(surfImage, 3);
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

    UnloadImage(alphaImage);
    UnloadImage(surfImage);
    return mask;
}

rg::mask::Mask
rg::mask::FromSurface(const Frames_Ptr &frames, const unsigned char threshold)
{
    auto mask = Mask(frames->render.texture.width, frames->render.texture.height);
    const rl::Image surfImage = LoadImageFromTextureSafe(frames->render.texture);
    const rl::Image alphaImage = ImageFromChannel(surfImage, 3);
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
            Rect{0, 0, (float) frames->render.texture.width, (float) frames->render.texture.height};

    UnloadImage(alphaImage);
    UnloadImage(surfImage);
    return mask;
}
