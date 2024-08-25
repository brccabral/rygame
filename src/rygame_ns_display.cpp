#include "rygame.hpp"


extern std::shared_ptr<rg::Surface> display_surface;

std::shared_ptr<rg::Surface> rg::display::SetMode(const int width, const int height)
{
    rl::InitWindow(width, height, "rygame");
    SetExitKey(rl::KEY_NULL);
    display_surface = std::make_shared<Surface>(width, height);
    return display_surface;
}

void rg::display::SetCaption(const char *title)
{
    rl::SetWindowTitle(title);
}

std::shared_ptr<rg::Surface> rg::display::GetSurface()
{
    return display_surface;
}

void rg::display::Update()
{
    for (const auto *music: musics)
    {
        UpdateMusicStream(*(rl::Music *) music->audio);
    }

    EndTextureModeSafe();
    // RenderTexture renders things flipped in Y axis, we draw it "unflipped"
    // https://github.com/raysan5/raylib/issues/3803
    TraceLog(rl::LOG_TRACE, rl::TextFormat("display::Update"));
    rl::BeginDrawing();
    DrawTextureRec(
            display_surface->GetTexture(),
            {0, 0, display_surface->atlas_rect.width, -display_surface->atlas_rect.height}, {0, 0},
            rl::WHITE);
#ifdef SHOW_FPS
    rl::DrawFPS(20, 20);
#endif
    rl::EndDrawing();
}
