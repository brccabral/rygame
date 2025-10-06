#include "rygame.hpp"


rg::Frames::Frames(const int width, const int height, const int rows, const int cols)
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
rg::Frames::Merge(const std::vector<Surface> &surfaces, const int rows, const int cols)
{
    if (surfaces.empty())
    {
        return {};
    }
    struct pixels
    {
        unsigned char r;
        unsigned char g;
        unsigned char b;
        unsigned char a;
    };
    const int singleWidth = surfaces[0].GetRect().width;
    const int singleHeight = surfaces[0].GetRect().height;
    auto result =
            Frames(singleWidth * cols, singleHeight * rows, rows, cols);
    const auto result_image = LoadImageFromTextureSafe(result.render.texture);
    auto *result_data = (pixels *) result_image.data;

    auto index = [cols, singleWidth, singleHeight](
            const int rc, const int rr, const int ic, const int ir)
    {
        const int fc = rc * singleWidth + ic;
        const int fr = rr * singleHeight + ir;
        return fr * (cols * singleWidth) + fc;
    };

    for (int r = 0; r < rows; ++r)
    {
        for (int c = 0; c < cols; ++c)
        {
            const unsigned int s = r * cols + c;
            const auto *surface = &surfaces[s];

            const rl::Image surfImage = LoadImageFromTextureSafe(surface->GetTexture());
            const auto *image_data = (pixels *) surfImage.data;
            for (int rr = 0; rr < surfImage.height; ++rr)
            {
                for (int cc = 0; cc < surfImage.width; ++cc)
                {
                    const auto id_i = rr * surfImage.width + cc;
                    const auto rd_i = index(c, r, cc, rr);
                    result_data[rd_i] = image_data[id_i];
                }
            }
        }
    }

    const auto result_texture = LoadTextureFromImageSafe(result_image);
    UnloadTextureSafe(result.render.texture);
    result.render.texture = result_texture;

    return result;
}

rg::Frames rg::Frames::Load(const char *file, const int rows, const int cols)
{
    const auto texture = LoadTextureSafe(file);

    auto result = Frames(texture.width, texture.height, rows, cols);
    UnloadTextureSafe(result.render.texture);
    result.render.texture = texture;
    result.atlas_rect.height *= -1;

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
    UnloadTextureSafe(render.texture);
    render.texture = color_texture;
    atlas_rect.height *= -1;

    // clean up
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
