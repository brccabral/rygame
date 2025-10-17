#include <rygame.hpp>

void rg::mouse::set_visible(const bool value)
{
    if (value)
    {
        rl::ShowCursor();
    }
    else
    {
        rl::HideCursor();
    }
}

rg::math::Vector2<float> rg::mouse::get_pos()
{
    return rg::math::Vector2<float>{rl::GetMousePosition()};
}
