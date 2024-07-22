#include "raylib_utils.h"
#include <algorithm>
#include <cstdio>
#include <cstring>
#include <filesystem>
#include <raymath.h>

using recursive_directory_iterator = std::filesystem::recursive_directory_iterator;

int current_render = 0;

void BeginTextureModeSafe(const RenderTexture2D &render)
{
    if (current_render > 0)
    {
        char text[MAX_TEXT_BUFFER_LENGTH];
        TextFormatSafe(text, "Double call to BeginTextureMode(), previous id %i new id %i", current_render, render.id);
        TraceLog(LOG_WARNING, text);
    }
    current_render = render.id;
    BeginTextureMode(render);
}

void EndTextureModeSafe()
{
    current_render = 0;
    EndTextureMode();
}

void BeginTextureModeC(const RenderTexture2D &render, const Color color)
{
    BeginTextureModeSafe(render);
    ClearBackground(color); // remove old memory
}

void BeginDrawingC(const Color color)
{
    BeginDrawing();
    ClearBackground(color);
}

void TextFormatSafe(char *buffer, const char *format, ...)
{
    std::memset(buffer, 0, MAX_TEXT_BUFFER_LENGTH); // Clear buffer before using

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

SpriteGroup::~SpriteGroup() = default;

void SpriteGroup::Draw(Surface *surface)
{
    for (const auto *sprite: sprites)
    {
        surface->Blit(sprite->image, sprite->rect.pos);
    }
}

void SpriteGroup::Update(const float deltaTime)
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

void SpriteGroup::DeleteAll()
{
    for (auto *sprite: sprites)
    {
        sprite->LeaveOtherGroups(this);
        delete sprite;
    }
}

SimpleSprite::SimpleSprite(SpriteGroup &sprite_group)
{
    groups.push_back(&sprite_group);
    sprite_group.sprites.push_back(this);
}

SimpleSprite::SimpleSprite(const std::vector<SpriteGroup *> &sprite_groups)
{
    for (auto *sprite_group: sprite_groups)
    {
        groups.push_back(sprite_group);
        sprite_group->sprites.push_back(this);
    }
}

SimpleSprite::~SimpleSprite()
{
    if (image)
    {
        delete image;
    }
}

void SimpleSprite::LeaveOtherGroups(const SpriteGroup *sprite_group)
{
    if (groups.empty())
    {
        return;
    }
    for (const auto group: groups)
    {
        if (group != sprite_group)
        {
            group->sprites.erase(std::remove(group->sprites.begin(), group->sprites.end(), this), group->sprites.end());
        }
    }
}

void SimpleSprite::Kill()
{
    // we add to another vector `to_delete` to delay the deletition to after
    // all group sprites Update(dt)
    if (!groups.empty())
    {
        groups[0]->to_delete.push_back(this);
    }
    for (const auto group: groups)
    {
        group->sprites.erase(std::remove(group->sprites.begin(), group->sprites.end(), this), group->sprites.end());
    }
    groups.clear();
}

void SimpleSprite::FlipH()
{
    image->atlas_rect.width = -image->atlas_rect.width;
}

Surface::Surface(const int width, const int height)
{
    render_texture = LoadRenderTexture(width, height);
    // IsRenderTextureReady does not retry nor waits for RenderTexture to be ready
    // in fact, it checks if it has an Id, Width and Height
    if (!IsRenderTextureReady(render_texture))
    {
        TraceLog(LOG_ERROR, "Could not load render_texture");
    }
    // RenderTexture draws textures upside-down
    atlas_rect = {0, 0, (float) render_texture.texture.width, (float) -render_texture.texture.height};

    // make sure surface is blank before drawing anything
    Fill(BLANK);
}

Surface::~Surface()
{
    UnloadRenderTexture(render_texture);
}

void Surface::Fill(const Color color) const
{
    BeginTextureModeC(render_texture, color);
    EndTextureModeSafe();
}

void Surface::Blit(Surface *surface, const Vector2 offset) const
{
    BeginTextureModeSafe(render_texture);
    DrawTextureRec(*surface->Texture(), surface->atlas_rect.rectangle, offset, WHITE);
    EndTextureModeSafe();
}

void Surface::Blit(const Texture2D *texture, const Vector2 offset, const RectangleU area) const
{
    BeginTextureModeSafe(render_texture);
    if (area.height)
    {
        DrawTextureRec(*texture, area.rectangle, offset, WHITE);
    }
    else
    {
        DrawTextureRec(*texture, {0, 0, (float) texture->width, (float) texture->height}, offset, WHITE);
    }
    EndTextureModeSafe();
}

RectangleU Surface::GetRect() const
{
    return {0, 0, abs(atlas_rect.width), abs(atlas_rect.height)};
}

Texture2D *Surface::Texture()
{
    return &render_texture.texture;
}

Surface *Surface::Load(const char *path)
{
    const Texture2D texture = LoadTexture(path);
    auto *surface = new Surface(texture.width, texture.height);
    BeginTextureModeSafe(surface->render_texture);
    DrawTextureV(texture, {0, 0}, WHITE);
    EndTextureModeSafe();
    UnloadTexture(texture);
    return surface;
}

Vector2 GetRectCenter(const RectangleU rect)
{
    return {rect.x + rect.width / 2, rect.y + rect.height / 2};
}

Vector2 GetRectMidBottom(const RectangleU rect)
{
    return {rect.x + rect.width / 2, rect.y + rect.height};
}

Vector2 GetRectMidTop(const RectangleU rect)
{
    return {rect.x + rect.width / 2, rect.y};
}

Vector2 GetRectMidLeft(const RectangleU rect)
{
    return {rect.x, rect.y + rect.height / 2};
}

Vector2 GetRectMidRight(const RectangleU rect)
{
    return {rect.x + rect.width, rect.y + rect.height / 2};
}

Vector2 GetRectTopLeft(const RectangleU rect)
{
    // return {rect.x, rect.y};
    return rect.pos;
}

Vector2 GetRectTopRight(const RectangleU rect)
{
    return {rect.x + rect.width, rect.y};
}

Vector2 GetRectBottomLeft(const RectangleU rect)
{
    return {rect.x, rect.y + rect.height};
}

Vector2 GetRectBottomRight(const RectangleU rect)
{
    return {rect.x + rect.width, rect.y + rect.height};
}

void RectToCenter(RectangleU &rect, const Vector2 pos)
{
    rect.x = pos.x - rect.width / 2;
    rect.y = pos.y - rect.height / 2;
}

void RectToMidBottom(RectangleU &rect, const Vector2 pos)
{
    rect.x = pos.x - rect.width / 2;
    rect.y = pos.y - rect.height;
}

void RectToMidLeft(RectangleU &rect, const Vector2 pos)
{
    rect.x = pos.x;
    rect.y = pos.y - rect.height / 2;
}

void RectToBottomLeft(RectangleU &rect, const Vector2 pos)
{
    rect.x = pos.x;
    rect.y = pos.y - rect.height;
}

void RectToTopLeft(RectangleU &rect, const Vector2 pos)
{
    // rect.x = pos.x;
    // rect.y = pos.y;
    rect.pos = pos;
}

void RectToTopRight(RectangleU &rect, const Vector2 pos)
{
    rect.x = pos.x - rect.width;
    rect.y = pos.y;
}

void RectInflate(RectangleU &rect, const float width, const float height)
{
    const Vector2 oldCenter = GetRectCenter(rect);
    rect.width += width;
    rect.height += height;
    RectToCenter(rect, oldCenter);
}

Vector2 operator+(const Vector2 &lhs, const Vector2 &rhs)
{
    return Vector2Add(lhs, rhs);
}

Vector2 operator*(const Vector2 &lhs, const float scale)
{
    return Vector2Scale(lhs, scale);
}

Vector2 &operator+=(Vector2 &lhs, const Vector2 &rhs)
{
    lhs.x += rhs.x;
    lhs.y += rhs.y;
    return lhs;
}

Vector2 &operator-=(Vector2 &lhs, const Vector2 &rhs)
{
    lhs.x -= rhs.x;
    lhs.y -= rhs.y;
    return lhs;
}

std::vector<Surface *> ImportFolder(const char *path)
{
    std::vector<Surface *> surfaces;
    for (const auto &dirEntry: recursive_directory_iterator(path))
    {
        auto entryPath = dirEntry.path().string();
        auto *surface = Surface::Load(entryPath.c_str());
        surfaces.push_back(surface);
    }
    return surfaces;
}

// duration is in seconds
Timer::Timer(const float duration, const bool repeat, const bool autostart, const std::function<void()> &func)
    : duration(duration), repeat(repeat), autostart(autostart), func(func)
{
    if (autostart)
    {
        Activate();
    }
}

void Timer::Activate()
{
    active = true;
    start_time = GetTime();
}

void Timer::Deactivate()
{
    active = false;
    start_time = GetTime();
    if (repeat)
    {
        Activate();
    }
}

void Timer::Update()
{
    if (!active)
    {
        return;
    }
    const double currentTime = GetTime();
    if (currentTime - start_time >= duration)
    {
        Deactivate();
        if (func)
        {
            func();
        }
    }
}

Surface *GetTMXTileSurface(const tmx_tile *tile)
{
    auto *surface = new Surface(tile->width, tile->height);

    const tmx_image *im = tile->image;
    const Texture2D *map_texture = nullptr;

    RectangleU srcRect;
    srcRect.x = tile->ul_x;
    srcRect.y = tile->ul_y;
    srcRect.width = tile->width;
    srcRect.height = tile->height;

    if (im && im->resource_image)
    {
        map_texture = (Texture2D *) im->resource_image;
    }
    else if (tile->tileset->image->resource_image)
    {
        map_texture = (Texture2D *) tile->tileset->image->resource_image;
    }
    if (map_texture)
    {
        surface->Blit(map_texture, {}, srcRect);
    }
    return surface;
}

std::vector<TileInfo> GetTMXTiles(const tmx_map *map, const tmx_layer *layer)
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
                const tmx_tileset *ts = map->tiles[gid]->tileset;
                auto *tileSurface = GetTMXTileSurface(map->tiles[gid]);
                Vector2 pos = {(float) x * ts->tile_width, (float) y * ts->tile_height};
                TileInfo tile_info = {pos, tileSurface};
                tiles.push_back(tile_info);
            }
        }
    }
    return tiles;
}

Surface *GetTMXLayerSurface(const tmx_map *map, const tmx_layer *layer)
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
