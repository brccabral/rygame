#include <rygame.hpp>


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

rg::font::Font::Font(Font &&other) noexcept : Font()
{
    *this = std::move(other);
}

rg::font::Font &rg::font::Font::operator=(Font &&other) noexcept
{
    if (this != &other)
    {
        rl::UnloadFont(font);
        font = other.font;
        font_size = other.font_size;
        other.font = rl::GetFontDefault(); // default font is unloaded in rl::CloseWindow()
    }
    return *this;
}

rg::font::Font::~Font()
{
    rl::UnloadFont(font);
}

rg::Surface rg::font::Font::render(
        const char *text, const rl::Color color, const float spacing, const rl::Color bg,
        const float padding_width, const float padding_height) const
{
    auto [w, h] = rl::MeasureTextEx(font, text, font_size, spacing);
    const int surfWidth = w + padding_width;
    const int surfHeight = h + padding_height;

    auto result = Surface(surfWidth, surfHeight);
    result.Fill(bg);
    auto f = font;
    auto fs = font_size;
    result.draw_cmds.emplace_back(
            [f, fs, text, padding_width, padding_height, spacing, color]
            {
                rl::DrawTextEx(
                        f, text, {padding_width / 2.0f, padding_height / 2.0f}, fs,
                        spacing, color);
            });
    result.Draw();

    return result;
}

rg::math::Vector2<float> rg::font::Font::size(const char *text) const
{
    const auto result = rl::MeasureTextEx(font, text, font_size, 1);
    return math::Vector2<float>{result};
}
