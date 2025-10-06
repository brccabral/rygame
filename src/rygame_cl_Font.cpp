#include "rygame.hpp"


rg::font::Font::Font(const float font_size) : font(rl::GetFontDefault()), font_size(font_size)
{
}

rg::font::Font::Font(const char *file, const float font_size)
    : font(rl::LoadFontEx(file, font_size, nullptr, 0)), font_size(font_size)
{
}

// rl:Font is trivial copiable
// ReSharper disable once CppPassValueParameterByConstReference
rg::font::Font::Font(rl::Font font, const float font_size) : font(font), font_size(font_size)
{
}

rg::font::Font::Font(Font &&other) noexcept : font(other.font), font_size(other.font_size)
{
    other.font = rl::GetFontDefault();
}

rg::font::Font &rg::font::Font::operator=(Font &&other) noexcept
{
    font = other.font;
    font_size = other.font_size;
    other.font = rl::GetFontDefault();
    return *this;
}

rg::font::Font::~Font()
{
    UnloadFont(font);
}

rg::Surface rg::font::Font::render(
        const char *text, const rl::Color color, const float spacing, const rl::Color bg,
        const float padding_width, const float padding_height) const
{
    // const rl::Image imageText = ImageTextEx(font, text, font_size, spacing, color);
    // const rl::Texture texture = LoadTextureFromImageSafe(imageText);
    //
    auto [w, h] = rl::MeasureTextEx(font, text, font_size, spacing);
    const int surfWidth = w + padding_width;
    const int surfHeight = h + padding_height;

    auto result = Surface(surfWidth, surfHeight);
    result.Fill(bg);
    // result.Blit(
    //         texture, {padding_width / 2.0f, padding_height / 2.0f},
    //         {0, 0, texture.width, -texture.height});
    result.draw_cmds.emplace_back(
            [this, text, padding_width, padding_height, spacing, color]
            {
                rl::DrawTextEx(
                        font, text, {padding_width / 2.0f, padding_height / 2.0f}, font_size,
                        spacing, color);
            });

    // UnloadTextureSafe(texture);
    // UnloadImage(imageText);
    return result;
}

rg::math::Vector2<float> rg::font::Font::size(const char *text) const
{
    auto result = MeasureTextEx(font, text, font_size, 1);
    return math::Vector2<float>{result};
}
