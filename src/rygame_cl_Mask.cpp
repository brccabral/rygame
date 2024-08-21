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

rg::mask::Mask::~Mask()
{
    UnloadImage(image);
}

std::shared_ptr<rg::Surface> rg::mask::Mask::ToSurface() const
{
    const rl::Texture2D maskTexture = LoadTextureFromImageSafe(image);
    const auto surface = std::make_shared<Surface>(image.width, image.height);
    surface->Fill(rl::BLANK);
    surface->Blit(maskTexture, {}, atlas_rect);
    UnloadTextureSafe(maskTexture);
    return surface;
}

std::shared_ptr<rg::Frames> rg::mask::Mask::ToFrames(int rows, int cols) const
{
    const rl::Texture2D maskTexture = LoadTextureFromImageSafe(image);
    const auto surface = std::make_shared<Frames>(image.width, image.height, rows, cols);
    surface->Fill(rl::BLANK);
    surface->Blit(maskTexture, {}, atlas_rect);
    surface->SetAtlas();
    UnloadTextureSafe(maskTexture);
    return surface;
}
