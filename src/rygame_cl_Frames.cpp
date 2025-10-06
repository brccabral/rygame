#include "rygame.hpp"


rg::Frames::Frames(const int width, const int height, int rows, int cols)
    : Surface(width, height), m_rows(rows), m_cols(cols)
{
    CreateFrames(width, height, rows, cols);
    atlas_rect = frames[current_frame_index];
    flip_atlas_height = -1;
}


rg::Frames::Frames(Surface *surface, const int rows, const int cols)
    : Frames(surface->GetRect().width, surface->GetRect().height, rows, cols)
{
    Fill(rl::BLANK);
    Blit(surface, math::Vector2{0.0f, 0.0f});
    flip_atlas_height = -1;
}

void rg::Frames::SetAtlas(const int frame_index)
{
    current_frame_index = (frame_index % (int) frames.size() + frames.size()) % frames.size();
    atlas_rect = frames[current_frame_index];
}

rg::Frames
rg::Frames::Merge(std::vector<Surface> &surfaces, const int rows, const int cols)
{
    if (surfaces.empty())
    {
        return {};
    }
    const int singleWidth = surfaces[0].GetRect().width;
    const int singleHeight = surfaces[0].GetRect().height;
    auto result =
            Frames(singleWidth * cols, singleHeight * rows, rows, cols);
    result.Fill(rl::BLANK);

    for (int r = 0; r < rows; ++r)
    {
        for (int c = 0; c < cols; ++c)
        {
            const unsigned int s = r * cols + c;
            result.Blit(
                    &surfaces[s], math::Vector2{(float) c * singleWidth, (float) r * singleHeight});
        }
    }

    return result;
}

rg::Frames rg::Frames::Load(const char *file, const int rows, const int cols)
{
    const auto texture = LoadTextureSafe(file);

    auto result = Frames(texture.width, texture.height, rows, cols);
    result.Fill(rl::BLANK);

    BeginTextureModeSafe(result.render);
    DrawTextureRec(
            texture, //
            {0, 0, (float) texture.width, -(float) texture.height}, //
            {0, 0}, rl::WHITE);

    UnloadTextureSafe(texture);
    return result;
}

void rg::Frames::SetColorKey(const rl::Color color)
{
    TraceLog(
            rl::LOG_TRACE,
            rl::TextFormat(
                    "Frames::SetColorKey render %d texture %d", render.id, render.texture.id));
    rl::Image current = LoadImageFromTextureSafe(render.texture);
    ImageColorReplace(&current, color, rl::BLANK);
    const rl::Texture color_texture = LoadTextureFromImageSafe(current);

    // replace
    Fill(rl::BLANK);
    Blit(color_texture, {}, {0, 0, render.texture.width, render.texture.height});

    // clean up
    UnloadTextureSafe(color_texture);
    UnloadImage(current);
}

rg::Frames rg::Frames::SubFrames(const Rect rect)
{
    const float frame_width = frames[0].width;
    const float frame_height = frames[0].height;
    const int rows = rect.height / frame_height;
    const int cols = rect.width / frame_width;

    auto result = Frames(GetTexture().width, GetTexture().height, rows, cols);
    UnloadRenderTextureSafe(result.render);
    result.render = render;
    result.shared_texture = shared_texture;
    result.parent = this;
    result.m_offset = rect.pos();

    result.frames.clear();
    for (const auto &frame: frames)
    {
        if (frame.colliderect(rect))
        {
            result.frames.push_back(frame);
        }
    }
    result.SetAtlas();

    return result;
}

void rg::Frames::CreateFrames(const int width, const int height, int rows, int cols)
{
    if (rows <= 0)
    {
        rows = 1;
    }
    if (cols <= 0)
    {
        cols = 1;
    }
    const float w = 1.0f * width / cols;
    const float h = 1.0f * height / rows;

    for (int r = 0; r < rows; ++r)
    {
        const float y = r * h + m_offset.y;
        for (int c = 0; c < cols; ++c)
        {
            const float x = c * w + m_offset.x;
            frames.push_back({x, y, w, h});
        }
    }
}
