#include "rygame.hpp"


rg::mask::Mask::Mask(const unsigned int width, const unsigned int height, const bool fill)
{
    auto *pixels = (unsigned char *) RL_CALLOC(width * height, sizeof(unsigned char));
    image.format = rl::PIXELFORMAT_UNCOMPRESSED_GRAYSCALE;
    image.width = width;
    image.height = height;
    image.mipmaps = 1;

    unsigned char bit = 0;
    if (fill)
    {
        bit = 255;
    }
    for (int i = 0; i < image.width * image.height; ++i)
    {
        pixels[i] = bit;
    }
    image.data = pixels;
}

rg::mask::Mask::Mask(Mask &&other) noexcept
    : Mask()
{
    *this = std::move(other);
}

rg::mask::Mask &rg::mask::Mask::operator=(Mask &&other) noexcept
{
    image = other.image;
    atlas_rect = other.atlas_rect;
    other.image.data = nullptr;
    return *this;
}

rg::mask::Mask::~Mask()
{
    UnloadImage(image);
}

rg::Surface rg::mask::Mask::ToSurface() const
{
    const auto gen = rl::GenImageColor(image.width, image.height, rl::BLANK);
    auto *gen_data = (rl::Color *) gen.data;
    const auto *mask_data = (unsigned char *) image.data;

    for (int i = 0; i < image.width * image.height; ++i)
    {
        if (mask_data[i] != 0)
        {
            gen_data[i] = rl::WHITE;
        }
        else
        {
            gen_data[i] = rl::BLACK;
        }
    }

    const auto maskTexture = LoadTextureFromImageSafe(gen);
    auto surface = Surface(image.width, image.height);
    UnloadTextureSafe(surface.render.texture);
    surface.render.texture = maskTexture;
    surface.atlas_rect.height *= -1;
    return surface;
}

rg::Frames rg::mask::Mask::ToFrames(const int rows, const int cols) const
{
    const rl::Texture2D maskTexture = LoadTextureFromImageSafe(image);
    auto surface = Frames(image.width, image.height, rows, cols);
    UnloadTextureSafe(surface.render.texture);
    surface.render.texture = maskTexture;
    surface.SetAtlas();
    return surface;
}
