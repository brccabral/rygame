#include "raylib_utils.h"
#include <algorithm>
#include <cstdio>
#include <cstring>
#include <raymath.h>

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

void SpriteGroup::Draw(const RenderTexture2D surface) const
{
    BeginTextureModeSafe(surface);
    for (const auto *sprite: sprites)
    {
        sprite->Draw({0, 0});
    }
    EndTextureModeSafe();
}

void SpriteGroup::Update(const double deltaTime)
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

SimpleSprite::SimpleSprite(const std::vector<SpriteGroup *> &sprite_groups)
{
    for (auto *sprite_group: sprite_groups)
    {
        groups.push_back(sprite_group);
        sprite_group->sprites.push_back(this);
    }
}

// the TMX images are unloaded in ~Game()::UnloadTMX
// if the sub class has its own render/texture, override
// the sub class destructor and unload it there
SimpleSprite::~SimpleSprite() = default;

void SimpleSprite::Draw(const Vector2 offset) const
{
    const Vector2 pos = Vector2Add(rect.pos, offset);

    if (image.texture)
    {
        DrawTextureRec(*image.texture, image.rect.rectangle, pos, WHITE);
    }
    else
    {
        DrawRectangle(pos.x, pos.y, rect.width, rect.height, BLACK);
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
    image.rect.width = -image.rect.width;
}
