#include <rygame.hpp>


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
    if (this != &other)
    {
        rl::UnloadImage(image);
        image = other.image;
        atlas_rect = other.atlas_rect;
        other.image.data = nullptr;
    }
    return *this;
}

rg::mask::Mask::~Mask()
{
    rl::UnloadImage(image);
}

rg::Surface rg::mask::Mask::ToSurface() const
{
    auto maskTexture = LoadTextureFromImageSafe(image);
    auto result = Surface(image.width, image.height);
    auto tempSurf = Surface(&maskTexture);
    result.Fill(rl::BLANK);
    result.Blit(&tempSurf, rg::math::Vector2<float>{});
    result.Draw();
    UnloadTextureSafe(maskTexture);
    return result;
}

rg::Frames rg::mask::Mask::ToFrames(const int rows, const int cols) const
{
    auto maskTexture = LoadTextureFromImageSafe(image);
    auto result = Frames(image.width, image.height, rows, cols);
    auto tempSurf = Surface(&maskTexture);
    result.Fill(rl::BLANK);
    result.Blit(&tempSurf, rg::math::Vector2<float>{});
    result.Draw();
    UnloadTextureSafe(maskTexture);
    result.SetAtlas();
    return result;
}
