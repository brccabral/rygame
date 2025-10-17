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
