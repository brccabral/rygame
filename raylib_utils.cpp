#include "raylib_utils.h"

int current_render = 0;
rg::Surface *display_surface = nullptr;

void rg::BeginTextureModeSafe(const rl::RenderTexture2D &render)
{
    if (current_render > 0)
    {
        char text[MAX_TEXT_BUFFER_LENGTH];
        TextFormatSafe(text, "Double call to BeginTextureMode(), previous id %i new id %i", current_render, render.id);
        TraceLog(rl::LOG_WARNING, text);
    }
    current_render = render.id;
    BeginTextureMode(render);
}

void rg::EndTextureModeSafe()
{
    current_render = 0;
    rl::EndTextureMode();
}

void rg::BeginTextureModeC(const rl::RenderTexture2D &render, const rl::Color color)
{
    BeginTextureModeSafe(render);
    ClearBackground(color); // remove old memory
}

void rg::BeginDrawingC(const rl::Color color)
{
    rl::BeginDrawing();
    ClearBackground(color);
}

void rg::TextFormatSafe(char *buffer, const char *format, ...)
{
    memset(buffer, 0, MAX_TEXT_BUFFER_LENGTH); // Clear buffer before using

    va_list args;
    va_start(args, format);
    const int requiredByteCount = vsnprintf(buffer, MAX_TEXT_BUFFER_LENGTH, format, args);
    va_end(args);

    // If requiredByteCount is larger than the MAX_TEXT_BUFFER_LENGTH, then overflow occured
    if (requiredByteCount >= MAX_TEXT_BUFFER_LENGTH)
    {
        // Inserting "..." at the end of the string to mark as truncated
        char *truncBuffer = buffer + MAX_TEXT_BUFFER_LENGTH - 4; // Adding 4 bytes = "...\0"
        sprintf(truncBuffer, "...");
    }
}

rg::sprite::Group::~Group() = default;

void rg::sprite::Group::Draw(Surface *surface)
{
    for (const auto *sprite: sprites)
    {
        surface->Blit(sprite->image, sprite->rect.pos);
    }
}

void rg::sprite::Group::Update(const float deltaTime)
{
    for (auto *sprite: sprites)
    {
        sprite->Update(deltaTime);
    }
    if (!to_delete.empty())
    {
        for (const auto *sprite: to_delete)
        {
            delete sprite;
        }
        to_delete.clear();
    }
}

void rg::sprite::Group::DeleteAll()
{
    for (auto *sprite: sprites)
    {
        sprite->LeaveOtherGroups(this);
        delete sprite;
    }
}

rg::sprite::Sprite::Sprite(Group &sprite_group)
{
    groups.push_back(&sprite_group);
    sprite_group.sprites.push_back(this);
}

rg::sprite::Sprite::Sprite(const std::vector<Group *> &sprite_groups)
{
    for (auto *sprite_group: sprite_groups)
    {
        groups.push_back(sprite_group);
        sprite_group->sprites.push_back(this);
    }
}

rg::sprite::Sprite::~Sprite()
{
    if (image)
    {
        delete image;
    }
}

void rg::sprite::Sprite::LeaveOtherGroups(const Group *sprite_group)
{
    if (groups.empty())
    {
        return;
    }
    for (const auto group: groups)
    {
        if (group != sprite_group)
        {
            group->sprites.erase(remove(group->sprites.begin(), group->sprites.end(), this), group->sprites.end());
        }
    }
}

void rg::sprite::Sprite::Kill()
{
    // we add to another vector `to_delete` to delay the deletition to after
    // all group sprites Update(dt)
    if (!groups.empty())
    {
        groups[0]->to_delete.push_back(this);
    }
    for (const auto group: groups)
    {
        group->sprites.erase(remove(group->sprites.begin(), group->sprites.end(), this), group->sprites.end());
    }
    groups.clear();
}

void rg::sprite::Sprite::FlipH()
{
    image->atlas_rect.width = -image->atlas_rect.width;
}

rg::Surface::Surface(const int width, const int height)
{
    render_texture = rl::LoadRenderTexture(width, height);
    // IsRenderTextureReady does not retry nor waits for RenderTexture to be ready
    // in fact, it checks if it has an Id, Width and Height
    if (!IsRenderTextureReady(render_texture))
    {
        TraceLog(rl::LOG_ERROR, "Could not load render_texture");
    }
    // RenderTexture draws textures upside-down
    atlas_rect = {0, 0, (float) render_texture.texture.width, (float) -render_texture.texture.height};

    // make sure surface is blank before drawing anything
    Fill(rl::BLANK);
}

rg::Surface::~Surface()
{
    UnloadRenderTexture(render_texture);
}

void rg::Surface::Fill(const rl::Color color) const
{
    BeginTextureModeC(render_texture, color);
    EndTextureModeSafe();
}

void rg::Surface::Blit(Surface *surface, const rl::Vector2 offset) const
{
    BeginTextureModeSafe(render_texture);
    DrawTextureRec(*surface->Texture(), surface->atlas_rect.rectangle, offset, rl::WHITE);
    EndTextureModeSafe();
}

void rg::Surface::Blit(const rl::Texture2D *texture, const rl::Vector2 offset, const RectangleU area) const
{
    BeginTextureModeSafe(render_texture);
    if (area.height)
    {
        DrawTextureRec(*texture, area.rectangle, offset, rl::WHITE);
    }
    else
    {
        DrawTextureRec(*texture, {0, 0, (float) texture->width, (float) texture->height}, offset, rl::WHITE);
    }
    EndTextureModeSafe();
}

rg::RectangleU rg::Surface::GetRect() const
{
    const float absWidth = atlas_rect.width > 0 ? atlas_rect.width : -atlas_rect.width;
    const float absHeight = atlas_rect.height > 0 ? atlas_rect.height : -atlas_rect.height;
    return {0, 0, absWidth, absHeight};
}

rl::Texture2D *rg::Surface::Texture()
{
    return &render_texture.texture;
}

rg::Surface *rg::Surface::Load(const char *path)
{
    const rl::Texture2D texture = rl::LoadTexture(path);
    auto *surface = new Surface(texture.width, texture.height);
    BeginTextureModeSafe(surface->render_texture);
    DrawTextureV(texture, {0, 0}, rl::WHITE);
    EndTextureModeSafe();
    UnloadTexture(texture);
    return surface;
}

rl::Vector2 rg::GetRectCenter(const RectangleU rect)
{
    return {rect.x + rect.width / 2, rect.y + rect.height / 2};
}

rl::Vector2 rg::GetRectMidBottom(const RectangleU rect)
{
    return {rect.x + rect.width / 2, rect.y + rect.height};
}

rl::Vector2 rg::GetRectMidTop(const RectangleU rect)
{
    return {rect.x + rect.width / 2, rect.y};
}

rl::Vector2 rg::GetRectMidLeft(const RectangleU rect)
{
    return {rect.x, rect.y + rect.height / 2};
}

rl::Vector2 rg::GetRectMidRight(const RectangleU rect)
{
    return {rect.x + rect.width, rect.y + rect.height / 2};
}

rl::Vector2 rg::GetRectTopLeft(const RectangleU rect)
{
    // return {rect.x, rect.y};
    return rect.pos;
}

rl::Vector2 rg::GetRectTopRight(const RectangleU rect)
{
    return {rect.x + rect.width, rect.y};
}

rl::Vector2 rg::GetRectBottomLeft(const RectangleU rect)
{
    return {rect.x, rect.y + rect.height};
}

rl::Vector2 rg::GetRectBottomRight(const RectangleU rect)
{
    return {rect.x + rect.width, rect.y + rect.height};
}

void rg::RectToCenter(RectangleU &rect, const rl::Vector2 pos)
{
    rect.x = pos.x - rect.width / 2;
    rect.y = pos.y - rect.height / 2;
}

void rg::RectToMidBottom(RectangleU &rect, const rl::Vector2 pos)
{
    rect.x = pos.x - rect.width / 2;
    rect.y = pos.y - rect.height;
}

void rg::RectToMidLeft(RectangleU &rect, const rl::Vector2 pos)
{
    rect.x = pos.x;
    rect.y = pos.y - rect.height / 2;
}

void rg::RectToBottomLeft(RectangleU &rect, const rl::Vector2 pos)
{
    rect.x = pos.x;
    rect.y = pos.y - rect.height;
}

void rg::RectToTopLeft(RectangleU &rect, const rl::Vector2 pos)
{
    // rect.x = pos.x;
    // rect.y = pos.y;
    rect.pos = pos;
}

void rg::RectToTopRight(RectangleU &rect, const rl::Vector2 pos)
{
    rect.x = pos.x - rect.width;
    rect.y = pos.y;
}

void rg::RectInflate(RectangleU &rect, const float width, const float height)
{
    const rl::Vector2 oldCenter = GetRectCenter(rect);
    rect.width += width;
    rect.height += height;
    RectToCenter(rect, oldCenter);
}

rl::Vector2 operator+(const rl::Vector2 &lhs, const rl::Vector2 &rhs)
{
    return rl::Vector2{lhs.x + rhs.x, lhs.y + rhs.y};
}

rl::Vector2 operator*(const rl::Vector2 &lhs, const float scale)
{
    return rl::Vector2{lhs.x * scale, lhs.y * scale};
}

void rg::Init(const int logLevel, const unsigned int config_flags, const rl::TraceLogCallback callback)
{
    rl::SetTraceLogLevel(logLevel);
    rl::SetConfigFlags(config_flags);
    rl::SetTraceLogCallback(callback);
    rl::SetRandomSeed(std::time(nullptr));
}

void rg::Quit()
{
    rl::CloseAudioDevice();
    rl::CloseWindow();
}

rl::Vector2 &operator+=(rl::Vector2 &lhs, const rl::Vector2 &rhs)
{
    lhs.x += rhs.x;
    lhs.y += rhs.y;
    return lhs;
}

rl::Vector2 &operator-=(rl::Vector2 &lhs, const rl::Vector2 &rhs)
{
    lhs.x -= rhs.x;
    lhs.y -= rhs.y;
    return lhs;
}

std::vector<rg::Surface *> rg::ImportFolder(const char *path)
{
    std::vector<rg::Surface *> surfaces;
    for (const auto &dirEntry: std::filesystem::recursive_directory_iterator(path))
    {
        auto entryPath = dirEntry.path().string();
        auto *surface = rg::Surface::Load(entryPath.c_str());
        surfaces.push_back(surface);
    }
    return surfaces;
}

// duration is in seconds
rg::Timer::Timer(const float duration, const bool repeat, const bool autostart, const std::function<void()> &func)
    : duration(duration), repeat(repeat), autostart(autostart), func(func)
{
    if (autostart)
    {
        Activate();
    }
}

void rg::Timer::Activate()
{
    active = true;
    start_time = rl::GetTime();
}

void rg::Timer::Deactivate()
{
    active = false;
    start_time = rl::GetTime();
    if (repeat)
    {
        Activate();
    }
}

void rg::Timer::Update()
{
    if (!active)
    {
        return;
    }
    const double currentTime = rl::GetTime();
    if (currentTime - start_time >= duration)
    {
        Deactivate();
        if (func)
        {
            func();
        }
    }
}

void rg::DrawRect(const Surface *surface, const rl::Color color, const RectangleU rect, const float lineThick)
{
    BeginTextureModeSafe(surface->render_texture);
    if (lineThick > 0)
    {
        DrawRectangleLinesEx(rect.rectangle, lineThick, color);
    }
    else if (lineThick == 0)
    {
        DrawRectangleV(rect.pos, rect.size, color);
    }
    EndTextureModeSafe();
}

void rg::DrawCirc(const Surface *surface, rl::Color color, rl::Vector2 center, float radius, float lineThick)
{
    BeginTextureModeSafe(surface->render_texture);
    if (lineThick > 0)
    {
        DrawCircleLinesV(center, radius, color);
    }
    else if (lineThick == 0)
    {
        DrawCircleV(center, radius, color);
    }
    EndTextureModeSafe();
}

rg::mask::Mask::Mask(const unsigned int width, const unsigned int height, const bool fill)
{
    auto *pixels = (unsigned char *) RL_CALLOC(width * height, sizeof(unsigned char));
    image.format = rl::PIXELFORMAT_UNCOMPRESSED_GRAYSCALE;
    image.width = width;
    image.height = height;
    image.mipmaps = 1;

    unsigned char bit = 0;
    if (fill)
    {
        bit = 255;
    }
    for (int i = 0; i < image.width * image.height; ++i)
    {
        pixels[i] = bit;
    }
    image.data = pixels;
}

rg::mask::Mask::~Mask()
{
    UnloadImage(image);
}

rg::Surface *rg::mask::Mask::ToSurface() const
{
    auto *surface = new Surface(image.width, image.height);
    const rl::Texture2D maskTexture = LoadTextureFromImage(image);
    surface->Blit(&maskTexture, {}, {0, 0, (float)image.width, (float)-image.height});
    UnloadTexture(maskTexture);
    return surface;
}

rg::mask::Mask rg::mask::FromSurface(Surface *surface, const unsigned char threshold)
{
    Mask mask = Mask(surface->Texture()->width, surface->Texture()->height);
    const rl::Image alphaImage = ImageFromChannel(LoadImageFromTexture(*surface->Texture()), 3);
    auto alphaData = (unsigned char *) alphaImage.data;
    auto maskData = (unsigned char *) mask.image.data;
    for (int i = 0; i < mask.image.width * mask.image.height; i++)
    {
        if (alphaData[i] > threshold)
        {
            maskData[i] = 255;
        }
    }

    UnloadImage(alphaImage);
    return mask;
}

rg::Surface *rg::display::SetMode(const int width, const int height)
{
    rl::InitWindow(width, height, "raygame");
    display_surface = new Surface(width, height);
    return display_surface;
}

void rg::display::SetCaption(const char *title)
{
    rl::SetWindowTitle(title);
}

rg::Surface *rg::display::GetSurface()
{
    return display_surface;
}

void rg::display::Update()
{
    // RenderTexture renders things flipped in Y axis, we draw it "unflipped"
    // https://github.com/raysan5/raylib/issues/3803
    rl::BeginDrawing();
    DrawTextureRec(
            *display_surface->Texture(),
            {0, 0, (float) display_surface->Texture()->width, (float) -display_surface->Texture()->height}, {0, 0},
            rl::WHITE);
    rl::EndDrawing();
}

// ReSharper disable once CppMemberFunctionMayBeStatic
float rg::time::Clock::tick(const int fps)
{
    if (fps)
    {
        rl::SetTargetFPS(fps);
    }
    return rl::GetFrameTime();
}

rg::Surface *rg::GetTMXTileSurface(const rl::tmx_tile *tile)
{
    auto *surface = new Surface(tile->width, tile->height);

    const rl::tmx_image *im = tile->image;
    const rl::Texture2D *map_texture = nullptr;

    RectangleU srcRect;
    srcRect.x = tile->ul_x;
    srcRect.y = tile->ul_y;
    srcRect.width = tile->width;
    srcRect.height = tile->height;

    if (im && im->resource_image)
    {
        map_texture = (rl::Texture2D *) im->resource_image;
    }
    else if (tile->tileset->image->resource_image)
    {
        map_texture = (rl::Texture2D *) tile->tileset->image->resource_image;
    }
    if (map_texture)
    {
        surface->Blit(map_texture, {}, srcRect);
    }
    return surface;
}

std::vector<rg::TileInfo> rg::GetTMXTiles(const rl::tmx_map *map, const rl::tmx_layer *layer)
{
    std::vector<TileInfo> tiles{};
    for (unsigned int y = 0; y < map->height; y++)
    {
        for (unsigned int x = 0; x < map->width; x++)
        {
            const unsigned int baseGid = layer->content.gids[(y * map->width) + x];
            const unsigned int gid = (baseGid) &TMX_FLIP_BITS_REMOVAL;
            if (map->tiles[gid])
            {
                const rl::tmx_tileset *ts = map->tiles[gid]->tileset;
                auto *tileSurface = GetTMXTileSurface(map->tiles[gid]);
                rl::Vector2 pos = {(float) x * ts->tile_width, (float) y * ts->tile_height};
                TileInfo tile_info = {pos, tileSurface};
                tiles.push_back(tile_info);
            }
        }
    }
    return tiles;
}

rg::Surface *rg::GetTMXLayerSurface(const rl::tmx_map *map, const rl::tmx_layer *layer)
{
    auto *surface = new Surface(map->width * map->tile_width, map->height * map->tile_height);
    std::vector<TileInfo> tiles = GetTMXTiles(map, layer);
    for (auto &[position, tileSurface]: tiles)
    {
        surface->Blit(tileSurface, position);
        delete tileSurface;
    }
    return surface;
}



rl::Image rg::GenImageRandomPixels(const float width, const float height)
{
    rl::Image image{};
    image.format = rl::PIXELFORMAT_UNCOMPRESSED_R8G8B8A8;
    image.height = height;
    image.width = width;
    image.mipmaps = 1;

    auto *pixels = (rl::Color *) RL_CALLOC(width * height, sizeof(rl::Color));
    for (int i = 0; i < width * height; i++)
    {
        pixels[i].r = rl::GetRandomValue(0, 255);
        pixels[i].g = rl::GetRandomValue(0, 255);
        pixels[i].b = rl::GetRandomValue(0, 255);
        pixels[i].a = rl::GetRandomValue(0, 255);
    }
    image.data = pixels;

    return image;
}
