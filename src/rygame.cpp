#include "rygame.h"

int current_render = 0;
rg::Surface *display_surface = nullptr;

class KilledSprites final : public rg::sprite::Group
{
public:

    KilledSprites() : Group(){};
    ~KilledSprites() override
    {
        DoDelete();
    };

    void DoDelete()
    {
        for (const auto *sprite: Sprites())
        {
            delete sprite;
        }
    };
};

KilledSprites killedSprites{};


void rg::Init(
        const int logLevel, const unsigned int config_flags, const rl::TraceLogCallback callback)
{
    rl::SetTraceLogLevel(logLevel);
    rl::SetConfigFlags(config_flags);
    rl::SetTraceLogCallback(callback);
    rl::SetRandomSeed(std::time(nullptr));
}

void rg::Quit()
{
    if (isSoundInit)
    {
        rl::CloseAudioDevice();
    }
    rl::CloseWindow();
}

void rg::BeginTextureModeSafe(const rl::RenderTexture2D &render)
{
    if (current_render > 0)
    {
        char text[MAX_TEXT_BUFFER_LENGTH];
        TextFormatSafe(
                text, "Double call to BeginTextureMode(), previous id %i new id %i", current_render,
                render.id);
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

void rg::TextFormatSafe(char *buffer, const char *format, ...)
{
    std::memset(buffer, 0, MAX_TEXT_BUFFER_LENGTH); // Clear buffer before using

    std::va_list args;
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

float rg::math::Vector2::operator[](const unsigned int &i) const
{
    if (i == 0)
    {
        return x;
    }
    if (i == 1)
    {
        return y;
    }
    throw;
}

float rg::Rect::right() const
{
    return x + width;
}

rg::Rect rg::Rect::right(const float v)
{
    x = v - width;
    return copy();
}

float rg::Rect::left() const
{
    return x;
}

rg::Rect rg::Rect::left(const float v)
{
    x = v;
    return copy();
}

float rg::Rect::centerx() const
{
    return x + width / 2.0f;
}

rg::Rect rg::Rect::centerx(const float v)
{
    x = v - width / 2.0f;
    return copy();
}

float rg::Rect::centery() const
{
    return y + height / 2.0f;
}

rg::Rect rg::Rect::centery(const float v)
{
    y = v - height / 2.0f;
    return copy();
}

rg::math::Vector2 rg::Rect::center() const
{
    return {x + width / 2.0f, y + height / 2.0f};
}

rg::Rect rg::Rect::center(const math::Vector2 pos)
{
    x = pos.x - width / 2.0f;
    y = pos.y - height / 2.0f;
    return copy();
}

float rg::Rect::top() const
{
    return y;
}

rg::Rect rg::Rect::top(const float v)
{
    y = v;
    return copy();
}

float rg::Rect::bottom() const
{
    return y + height;
}

rg::Rect rg::Rect::bottom(const float v)
{
    y = v - height;
    return copy();
}

rg::math::Vector2 rg::Rect::topleft() const
{
    return {x, y};
}

rg::Rect rg::Rect::topleft(const math::Vector2 pos)
{
    x = pos.x;
    y = pos.y;
    return copy();
}

rg::math::Vector2 rg::Rect::bottomleft() const
{
    return {x, y + height};
}

rg::Rect rg::Rect::bottomleft(const math::Vector2 pos)
{
    x = pos.x;
    y = pos.y - height;
    return copy();
}

rg::math::Vector2 rg::Rect::topright() const
{
    return {x + width, y};
}

rg::Rect rg::Rect::topright(const math::Vector2 pos)
{
    x = pos.x - width;
    y = pos.y;
    return copy();
}

rg::math::Vector2 rg::Rect::bottomright() const
{
    return {x + width, y + height};
}

rg::Rect rg::Rect::bottomright(const math::Vector2 pos)
{
    x = pos.x - width;
    y = pos.y - height;
    return copy();
}

rg::math::Vector2 rg::Rect::midbottom() const
{
    return {x + width / 2.0f, y + height};
}

rg::Rect rg::Rect::midbottom(const math::Vector2 pos)
{
    x = pos.x - width / 2.0f;
    y = pos.y - height;
    return copy();
}

rg::math::Vector2 rg::Rect::midtop() const
{
    return {x + width / 2.0f, y};
}

rg::Rect rg::Rect::midtop(const math::Vector2 pos)
{
    x = pos.x - width / 2.0f;
    y = pos.y;
    return copy();
}

rg::math::Vector2 rg::Rect::midleft() const
{
    return {x, y + height / 2.0f};
}

rg::Rect rg::Rect::midleft(const math::Vector2 pos)
{
    x = pos.x;
    y = pos.y - height / 2.0f;
    return copy();
}

rg::math::Vector2 rg::Rect::midright() const
{
    return {x + width, y + height / 2.0f};
}

rg::Rect rg::Rect::midright(const math::Vector2 pos)
{
    x = pos.x - width;
    y = pos.y - height / 2.0f;
    return copy();
}

rg::Rect rg::Rect::inflate(const float width, const float height) const
{
    Rect result{x, y, this->width, this->height};
    result.inflate_ip(width, height);
    return result;
}

rg::Rect rg::Rect::scale_by(const float ratio) const
{
    Rect result{x, y, this->width, this->height};
    result.scale_by_ip(ratio);
    return result;
}

void rg::Rect::inflate_ip(const float width, const float height)
{
    const math::Vector2 oldCenter = center();
    this->width += width;
    this->height += height;
    center(oldCenter);
}

void rg::Rect::scale_by_ip(const float ratio)
{
    const math::Vector2 oldCenter = center();
    this->width *= ratio;
    this->height *= ratio;
    center(oldCenter);
}

rg::Rect rg::Rect::copy() const
{
    return {x, y, width, height};
}

bool rg::Rect::collidepoint(const math::Vector2 point) const
{
    return CheckCollisionPointRec(point.vector2, rectangle);
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
    atlas_rect = {
            0, 0, (float) render_texture.texture.width, (float) -render_texture.texture.height};

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

void rg::Surface::Blit(
        Surface *surface, const math::Vector2 offset, const rl::BlendMode blend_mode) const
{
    if (!surface)
    {
        return;
    }
    BeginTextureModeSafe(render_texture);
    if (blend_mode != rl::BLEND_ALPHA)
    {
        BeginBlendMode(blend_mode);
    }

    DrawTextureRec(*surface->Texture(), surface->atlas_rect.rectangle, offset.vector2, rl::WHITE);

    if (blend_mode != rl::BLEND_ALPHA)
    {
        rl::EndBlendMode();
    }
    EndTextureModeSafe();
}

void rg::Surface::Blit(
        const rl::Texture2D *texture, const math::Vector2 offset, const Rect area,
        const rl::BlendMode blend_mode) const
{
    if (!texture)
    {
        return;
    }
    BeginTextureModeSafe(render_texture);
    if (blend_mode != rl::BLEND_ALPHA)
    {
        BeginBlendMode(blend_mode);
    }

    if (area.height)
    {
        DrawTextureRec(*texture, area.rectangle, offset.vector2, rl::WHITE);
    }
    else
    {
        DrawTexture(*texture, offset.x, offset.y, rl::WHITE);
    }

    if (blend_mode != rl::BLEND_ALPHA)
    {
        rl::EndBlendMode();
    }
    EndTextureModeSafe();
}

rg::Rect rg::Surface::GetRect() const
{
    const float absWidth = atlas_rect.width > 0 ? atlas_rect.width : -atlas_rect.width;
    const float absHeight = atlas_rect.height > 0 ? atlas_rect.height : -atlas_rect.height;
    return {0, 0, absWidth, absHeight};
}

rl::Texture2D *rg::Surface::Texture()
{
    return &render_texture.texture;
}

void rg::Surface::SetColorKey(const rl::Color color) const
{
    rl::Image current = LoadImageFromTexture(render_texture.texture);
    ImageColorReplace(&current, color, rl::BLANK);
    Fill(rl::BLANK);
    const rl::Texture texture = LoadTextureFromImage(current);
    Blit(&texture);
    UnloadImage(current);
    UnloadTexture(texture);
}

rg::Surface *rg::Surface::Load(const char *path)
{
    const rl::Texture2D texture = rl::LoadTexture(path);
    // ReSharper disable once CppDFAMemoryLeak
    auto *surface = new Surface(texture.width, texture.height);
    BeginTextureModeC(surface->render_texture, rl::BLANK);
    DrawTextureV(texture, {0, 0}, rl::WHITE);
    EndTextureModeSafe();
    UnloadTexture(texture);
    return surface;
}

void rg::draw::rect(
        const Surface *surface, const rl::Color color, const Rect rect, const float lineThick,
        const float radius)
{
    BeginTextureModeSafe(surface->render_texture);
    if (lineThick > 0)
    {
        if (radius > 0)
        {
            const int segments = (rect.width > rect.height ? rect.width : rect.height) * radius;
            DrawRectangleRoundedLinesEx(rect.rectangle, 0.5f, segments, lineThick, color);
        }
        else
        {
            DrawRectangleLinesEx(rect.rectangle, lineThick, color);
        }
    }
    else if (lineThick == 0)
    {
        if (radius > 0)
        {
            const int segments = (rect.width > rect.height ? rect.width : rect.height) * radius;
            DrawRectangleRounded(rect.rectangle, 0.5f, segments, color);
        }
        else
        {
            DrawRectangleV(rect.pos.vector2, rect.size.vector2, color);
        }
    }
    EndTextureModeSafe();
}

void rg::draw::circle(
        const Surface *surface, const rl::Color color, const math::Vector2 center,
        const float radius, const float lineThick)
{
    BeginTextureModeSafe(surface->render_texture);
    if (lineThick > 0)
    {
        DrawCircleLinesV(center.vector2, radius, color);
    }
    else if (lineThick == 0)
    {
        DrawCircleV(center.vector2, radius, color);
    }
    EndTextureModeSafe();
}

std::vector<rg::Surface *> rg::assets::ImportFolder(const char *path)
{
    std::vector<Surface *> surfaces;
    for (const auto &dirEntry: std::filesystem::recursive_directory_iterator(path))
    {
        auto entryPath = dirEntry.path().string();
        auto *surface = Surface::Load(entryPath.c_str());
        surfaces.push_back(surface);
    }
    return surfaces;
}

std::map<std::string, rg::Surface *> rg::assets::ImportFolderDict(const char *path)
{
    std::map<std::string, Surface *> result;
    for (const auto &dirEntry: std::filesystem::recursive_directory_iterator(path))
    {
        auto entryPath = dirEntry.path().string();
        auto filename = dirEntry.path().stem().string();
        auto *surface = Surface::Load(entryPath.c_str());
        result[filename] = surface;
    }
    // ReSharper disable once CppDFAMemoryLeak
    return result;
}

rg::Surface *rg::tmx::GetTMXTileSurface(const rl::tmx_tile *tile)
{
    auto *surface = new Surface(tile->width, tile->height);

    const rl::tmx_image *im = tile->image;
    const rl::Texture2D *map_texture = nullptr;

    Rect srcRect;
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

std::vector<rg::tmx::TileInfo>
rg::tmx::GetTMXTiles(const rl::tmx_map *map, const rl::tmx_layer *layer)
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
                const math::Vector2 pos = {(float) x * ts->tile_width, (float) y * ts->tile_height};
                TileInfo tile_info = {pos, tileSurface};
                tiles.push_back(tile_info);
            }
        }
    }
    return tiles;
}

rg::Surface *rg::tmx::GetTMXLayerSurface(const rl::tmx_map *map, const rl::tmx_layer *layer)
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

rg::sprite::Group::~Group()
{
    for (const auto *sprite: Sprites())
    {
        delete sprite;
    }
};

void rg::sprite::Group::Draw(Surface *surface)
{
    for (const auto *sprite: sprites)
    {
        surface->Blit(sprite->image, sprite->rect.pos);
    }
}

void rg::sprite::Group::Update(const float deltaTime)
{
    for (auto *sprite: Sprites())
    {
        sprite->Update(deltaTime);
    }
}

void rg::sprite::Group::empty()
{
    for (auto *sprite: Sprites())
    {
        sprite->remove(this);
    }
    sprites.clear();
}

void rg::sprite::Group::remove(const std::vector<Sprite *> &to_remove_sprites)
{
    for (auto *sprite: to_remove_sprites)
    {
        remove(sprite);
    }
}

void rg::sprite::Group::remove( // NOLINT(*-no-recursion) - the recursion is broken with has()
        Sprite *to_remove_sprite)
{
    if (has(to_remove_sprite))
    {
        sprites.erase(std::remove(sprites.begin(), sprites.end(), to_remove_sprite), sprites.end());
        to_remove_sprite->remove(this);
    }
}

void rg::sprite::Group::add(const std::vector<Sprite *> &to_add_sprites)
{
    for (auto *sprite: to_add_sprites)
    {
        add(sprite);
    }
}


void rg::sprite::Group::add( // NOLINT(*-no-recursion) - the recursion is broken with has()
        Sprite *to_add_sprite)
{
    if (!has(to_add_sprite))
    {
        sprites.push_back(to_add_sprite);
        to_add_sprite->add(this);
    }
}

bool rg::sprite::Group::has(const std::vector<Sprite *> &check_sprites)
{
    for (const auto *sprite: check_sprites)
    {
        if (!has(sprite))
        {
            return false;
        }
    }
    return true;
}

bool rg::sprite::Group::has(const Sprite *check_sprite)
{
    return std::find(sprites.begin(), sprites.end(), check_sprite) != sprites.end();
}


std::vector<rg::sprite::Sprite *> rg::sprite::Group::Sprites()
{
    return sprites;
}

rg::sprite::Sprite::Sprite(Group *to_add_group)
{
    if (to_add_group)
    {
        add(to_add_group);
    }
}

rg::sprite::Sprite::Sprite(const std::vector<Group *> &groups)
{
    add(groups);
}

rg::sprite::Sprite::~Sprite()
{
    LeaveAllGroups();
    delete image;
}

void rg::sprite::Sprite::add( // NOLINT(*-no-recursion) - the recursion is broken with has()
        Group *to_add_group)
{
    if (to_add_group)
    {
        if (!has(to_add_group))
        {
            groups.push_back(to_add_group);
            to_add_group->add(this);
        }
    }
}

void rg::sprite::Sprite::add(const std::vector<Group *> &to_add_groups)
{
    for (auto *sprite_group: to_add_groups)
    {
        add(sprite_group);
    }
}

void rg::sprite::Sprite::remove( // NOLINT(*-no-recursion) - the recursion is broken with has()
        Group *to_remove_group)
{
    if (has(to_remove_group))
    {
        groups.erase(std::remove(groups.begin(), groups.end(), to_remove_group), groups.end());
        to_remove_group->remove(this);
    }
}

void rg::sprite::Sprite::remove(const std::vector<Group *> &to_remove_groups)
{
    for (auto *to_remove_group: to_remove_groups)
    {
        remove(to_remove_group);
    }
}

bool rg::sprite::Sprite::has(const Group *check_group)
{
    return std::find(groups.begin(), groups.end(), check_group) != groups.end();
}

std::vector<rg::sprite::Group *> rg::sprite::Sprite::Groups()
{
    return groups;
}

void rg::sprite::Sprite::LeaveOtherGroups(const Group *not_leave_group)
{
    for (const auto group: Groups())
    {
        if (group != not_leave_group)
        {
            group->remove(this);
        }
    }
}

void rg::sprite::Sprite::LeaveAllGroups() // NOLINT(*-no-recursion) - the recursion does not happen
                                          // because we pass `false`
{
    // leave all groups
    for (const auto group: Groups())
    {
        group->remove(this);
    }
    // it doesn't belong to any group
    groups.clear();
}

rg::sprite::Sprite *rg::sprite::Sprite::Kill(const bool deleteSprite)
{
    // leave all groups
    LeaveAllGroups();

    if (deleteSprite)
    {
        killedSprites.add(this);
        return nullptr;
    }
    return this;
}

void rg::sprite::Sprite::FlipH()
{
    image->atlas_rect.width = -image->atlas_rect.width;
}

bool rg::sprite::collide_rect(const Sprite *left, const Sprite *right)
{
    return CheckCollisionRecs(left->rect.rectangle, right->rect.rectangle);
}

rg::sprite::collide_rect_ratio::collide_rect_ratio(const float ratio) : ratio(ratio)
{}

bool rg::sprite::collide_rect_ratio::operator()(const Sprite *left, const Sprite *right) const
{
    Rect leftrect = left->rect;
    Rect rightrect = right->rect;

    leftrect.scale_by_ip(ratio);
    rightrect.scale_by_ip(ratio);

    return collide_rect(left, right);
}

std::vector<rg::sprite::Sprite *> rg::sprite::spritecollide(
        Sprite *sprite, Group *group, const bool dokill,
        const std::function<bool(Sprite *left, Sprite *right)> &collided)
{
    std::vector<Sprite *> result;
    for (auto *other_sprite: group->Sprites())
    {
        if (collided(sprite, other_sprite))
        {
            result.push_back(other_sprite);
            if (dokill)
            {
                // just remove from group, don't delete
                // it will be returned in the result
                // if needed, delete it in the vector later
                other_sprite->Kill(false);
            }
        }
    }
    return result;
}

rg::sprite::Sprite *rg::sprite::spritecollideany(
        Sprite *sprite, Group *group,
        const std::function<bool(Sprite *left, Sprite *right)> &collided)
{
    for (auto *other_sprite: group->Sprites())
    {
        if (collided(sprite, other_sprite))
        {
            return other_sprite;
        }
    }
    return nullptr;
}

// duration is in seconds
rg::Timer::Timer(
        const float duration, const bool repeat, const bool autostart,
        const std::function<void()> &func)
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

rg::mixer::Sound::Sound(const char *file, const bool isMusic) : isMusic(isMusic), file(file)
{
    if (!isSoundInit)
    {
        rl::InitAudioDevice();
        isSoundInit = rl::IsAudioDeviceReady();
    }
    if (isMusic)
    {
        audio = new rl::Music;
        (*(rl::Music *) audio) = rl::LoadMusicStream(file);
        musics.push_back(this);
    }
    else
    {
        audio = new rl::Sound;
        (*(rl::Sound *) audio) = rl::LoadSound(file);
    }
}

rg::mixer::Sound::~Sound()
{
    if (isMusic)
    {
        musics.erase(std::remove(musics.begin(), musics.end(), this), musics.end());
        UnloadMusicStream(*(rl::Music *) audio);
        delete (rl::Music *) audio;
    }
    else
    {
        UnloadSound(*(rl::Sound *) audio);
        delete (rl::Sound *) audio;
    }
}

void rg::mixer::Sound::Play() const
{
    if (isMusic)
    {
        PlayMusicStream(*(rl::Music *) audio);
    }
    else
    {
        PlaySound(*(rl::Sound *) audio);
    }
}

void rg::mixer::Sound::Stop() const
{
    if (isMusic)
    {
        if(IsMusicStreamPlaying(*(rl::Music *) audio))
        {
            StopMusicStream(*(rl::Music *) audio);
        }
    }
    else
    {
        if(IsSoundPlaying(*(rl::Sound *) audio))
        {
            StopSound(*(rl::Sound *) audio);
        }
    }
}

void rg::mixer::Sound::SetVolume(const float value) const
{
    if (isMusic)
    {
        SetMusicVolume(*(rl::Music *) audio, value);
    }
    else
    {
        SetSoundVolume(*(rl::Sound *) audio, value);
    }
}

rg::Surface *rg::display::SetMode(const int width, const int height)
{
    rl::InitWindow(width, height, "rygame");
    SetExitKey(rl::KEY_NULL);
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
    killedSprites.DoDelete();
    for (const auto *music: musics)
    {
        UpdateMusicStream(*(rl::Music *) music->audio);
    }

    // RenderTexture renders things flipped in Y axis, we draw it "unflipped"
    // https://github.com/raysan5/raylib/issues/3803
    rl::BeginDrawing();
    DrawTextureRec(
            *display_surface->Texture(),
            {0, 0, (float) display_surface->Texture()->width,
             (float) -display_surface->Texture()->height},
            {0, 0}, rl::WHITE);
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
    surface->Blit(&maskTexture);
    UnloadTexture(maskTexture);
    return surface;
}

rg::mask::Mask rg::mask::FromSurface(Surface *surface, const unsigned char threshold)
{
    auto mask = Mask(surface->Texture()->width, surface->Texture()->height);
    const rl::Image surfImage = LoadImageFromTexture(*surface->Texture());
    const rl::Image alphaImage = ImageFromChannel(surfImage, 3);
    const auto alphaData = (unsigned char *) alphaImage.data;
    const auto maskData = (unsigned char *) mask.image.data;
    for (int i = 0; i < mask.image.width * mask.image.height; i++)
    {
        if (alphaData[i] > threshold)
        {
            maskData[i] = 255;
        }
    }

    UnloadImage(alphaImage);
    UnloadImage(surfImage);
    return mask;
}

rg::font::Font::Font() : font(rl::GetFontDefault()), size(1)
{}

rg::font::Font::Font(const char *file, const float size)
    : font(rl::LoadFontEx(file, size, nullptr, 0)), size(size)
{}

// rl:Font is trivial copiable
// ReSharper disable once CppPassValueParameterByConstReference
rg::font::Font::Font(rl::Font font, const float size) : font(font), size(size)
{}

rg::font::Font::~Font()
{
    UnloadFont(font);
}

rg::Surface *
rg::font::Font::render(const char *text, const rl::Color color, const float spacing) const
{
    const rl::Image imageText = ImageTextEx(font, text, size, spacing, color);
    const rl::Texture texture = LoadTextureFromImage(imageText);

    auto *result = new Surface(imageText.width, imageText.height);
    result->Blit(&texture);

    UnloadTexture(texture);
    UnloadImage(imageText);
    return result;
}

rg::math::Vector2 operator+(const rg::math::Vector2 &lhs, const rg::math::Vector2 &rhs)
{
    return rg::math::Vector2{lhs.x + rhs.x, lhs.y + rhs.y};
}

rg::math::Vector2 operator-(const rg::math::Vector2 &lhs, const rg::math::Vector2 &rhs)
{
    return rg::math::Vector2{lhs.x - rhs.x, lhs.y-rhs.y};
}

rg::math::Vector2 operator*(const rg::math::Vector2 &lhs, const float scale)
{
    return rg::math::Vector2{lhs.x * scale, lhs.y * scale};
}

rg::math::Vector2 &operator+=(rg::math::Vector2 &lhs, const rg::math::Vector2 &rhs)
{
    lhs.x += rhs.x;
    lhs.y += rhs.y;
    return lhs;
}

rg::math::Vector2 &operator-=(rg::math::Vector2 &lhs, const rg::math::Vector2 &rhs)
{
    lhs.x -= rhs.x;
    lhs.y -= rhs.y;
    return lhs;
}

float rg::math::Vector2::magnitude() const
{
    return Vector2Length(vector2);
}

rg::math::Vector2 rg::math::Vector2::normalize() const
{
    return {Vector2Normalize(vector2)};
}

void rg::math::Vector2::normalize_ip()
{
    vector2 = Vector2Normalize(vector2);
}
