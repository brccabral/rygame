#include "rygame.hpp"


rg::Frames::Frames(const int width, const int height, int rows, int cols)
    : Surface(width, height), rows(rows), cols(cols)
{
    CreateFrames(width, height, rows, cols);
    atlas_rect = frames[current_frame_index];
    flip_atlas_height = -1;
}


rg::Frames::Frames(const Surface_Ptr &surface, const int rows, const int cols)
    : Frames(surface->GetRect().width, surface->GetRect().height, rows, cols)
{
    Fill(rl::BLANK);
    Blit(surface, math::Vector2{});
    flip_atlas_height = -1;
}

void rg::Frames::SetAtlas(const int frame_index)
{
    current_frame_index = (frame_index % (int) frames.size() + frames.size()) % frames.size();
    atlas_rect = frames[current_frame_index];
}

rg::Frames_Ptr
rg::Frames::Merge(const std::vector<Surface_Ptr> &surfaces, const int rows, const int cols)
{
    if (surfaces.empty())
    {
        return nullptr;
    }
    const int singleWidth = surfaces[0]->GetRect().width;
    const int singleHeight = surfaces[0]->GetRect().height;
    const auto result =
            std::make_shared<Frames>(singleWidth * cols, singleHeight * rows, rows, cols);
    result->Fill(rl::BLANK);

    for (int r = 0; r < rows; ++r)
    {
        for (int c = 0; c < cols; ++c)
        {
            const unsigned int s = r * cols + c;
            result->Blit(
                    surfaces[s], math::Vector2{(float) c * singleWidth, (float) r * singleHeight});
        }
    }

    return result;
}

rg::Frames_Ptr rg::Frames::Load(const char *file, int rows, int cols)
{
    auto texture = LoadTextureSafe(file);

    auto result = std::make_shared<Frames>(texture.width, texture.height, rows, cols);
    result->Fill(rl::BLANK);

    BeginTextureModeSafe(result->render);
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
    Blit(color_texture, {}, {0, 0, (float) render.texture.width, (float) render.texture.height});

    // clean up
    UnloadTextureSafe(color_texture);
    UnloadImage(current);
}

rg::Frames_Ptr rg::Frames::SubFrames(const Rect rect)
{
    const float frame_width = frames[0].width;
    const float frame_height = frames[0].height;
    int rows = rect.height / frame_height;
    int cols = rect.width / frame_width;

    auto result = std::make_shared<Frames>(GetTexture().width, GetTexture().height, rows, cols);
    UnloadRenderTextureSafe(result->render);
    result->render = render;
    result->shared_texture = shared_texture;
    result->parent = shared_from_this();
    result->offset = rect.pos;

    result->frames.clear();
    for (const auto &frame: frames)
    {
        if (frame.colliderect(rect))
        {
            result->frames.push_back(frame);
        }
    }
    result->SetAtlas();

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
        const float y = r * h + offset.y;
        for (int c = 0; c < cols; ++c)
        {
            const float x = c * w + offset.x;
            frames.push_back({x, y, w, h});
        }
    }
}
