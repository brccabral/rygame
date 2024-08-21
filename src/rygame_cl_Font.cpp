#include "rygame.hpp"


rg::font::Font::Font(const float font_size) : font(rl::GetFontDefault()), font_size(font_size)
{}

rg::font::Font::Font(const char *file, const float font_size)
    : font(rl::LoadFontEx(file, font_size, nullptr, 0)), font_size(font_size)
{}

// rl:Font is trivial copiable
// ReSharper disable once CppPassValueParameterByConstReference
rg::font::Font::Font(rl::Font font, const float font_size) : font(font), font_size(font_size)
{}

rg::font::Font::~Font()
{
    UnloadFont(font);
}

std::shared_ptr<rg::Surface> rg::font::Font::render(
        const char *text, const rl::Color color, const float spacing, const rl::Color bg,
        const float padding_width, const float padding_height) const
{
    TraceLog(rl::LOG_TRACE, rl::TextFormat("Font::render %s", text));
    const rl::Image imageText = ImageTextEx(font, text, font_size, spacing, color);
    const rl::Texture texture = LoadTextureFromImageSafe(imageText);

    const int surfWidth = imageText.width + padding_width;
    const int surfHeight = imageText.height + padding_height;

    const auto result = std::make_shared<Surface>(surfWidth, surfHeight);
    result->Fill(bg);
    result->Blit(
            texture, {padding_width / 2.0f, padding_height / 2.0f},
            {0, 0, (float) texture.width, -(float) texture.height});

    UnloadTextureSafe(texture);
    UnloadImage(imageText);
    return result;
}

rg::math::Vector2 rg::font::Font::size(const char *text) const
{
    return {MeasureTextEx(font, text, font_size, 1)};
}
