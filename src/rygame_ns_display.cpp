#include "rygame.hpp"
#include "rygame_cl_Rygame.hpp"


extern Rygame *rygame;

rg::Surface &rg::display::SetMode(const int width, const int height)
{
    if (!rygame->isInit)
    {
        rg::Init();
    }
    int w = width;
    if (width == 0)
    {
        w = rl::GetMonitorWidth(rl::GetCurrentMonitor());
    }
    int h = height;
    if (height == 0)
    {
        h = rl::GetMonitorWidth(rl::GetCurrentMonitor());
    }
    rl::InitWindow(w, h, "rygame");
    SetExitKey(rl::KEY_NULL);
    rygame->display_surface = Surface(width, height);
    return rygame->display_surface;
}

void rg::display::SetCaption(const char *title)
{
    rl::SetWindowTitle(title);
}

rg::Surface &rg::display::GetSurface()
{
    return rygame->display_surface;
}

void rg::display::Update()
{
    for (const auto *music: rygame->musics)
    {
        UpdateMusicStream(music->audio.music);
    }

    EndTextureModeSafe();
    // RenderTexture renders things flipped in Y axis, we draw it "unflipped"
    // https://github.com/raysan5/raylib/issues/3803
    TraceLog(rl::LOG_TRACE, rl::TextFormat("display::Update"));
    rl::BeginDrawing();
    DrawTextureRec(
            rygame->display_surface.GetTexture(),
            {0, 0, rygame->display_surface.atlas_rect.width,
             -rygame->display_surface.atlas_rect.height},
            {0, 0}, rl::WHITE);
#ifdef SHOW_FPS
    rl::DrawFPS(20, 20);
#endif
    rl::EndDrawing();
}
