#include "rygame.hpp"
#include <cassert>

static bool isSoundInit = false;
static unsigned int current_render = 0; // TODO make it stack<> ?

std::random_device rd{};
std::mt19937 gen(rd());

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
    display::display_surface.reset();
    if (isSoundInit)
    {
        rl::CloseAudioDevice();
    }
    rl::CloseWindow();
}

void rg::BeginTextureModeSafe(const rl::RenderTexture2D &render)
{
    if (current_render == render.id)
    {
        return;
    }
    if (current_render)
    {
        char text[MAX_TEXT_BUFFER_LENGTH];
        TextFormatSafe(
                text, "Double call to BeginTextureMode(), previous id %i new id %i", current_render,
                render.id);
        TraceLog(rl::LOG_WARNING, text);
        EndTextureModeSafe();
    }
    current_render = render.id;
    BeginTextureMode(render);
}

void rg::EndTextureModeSafe()
{
    if (current_render)
    {
        TraceLog(rl::LOG_DEBUG, rl::TextFormat("End render %d", current_render));
        rl::EndTextureMode();
    }
    current_render = 0;
}

rl::Texture2D rg::LoadTextureSafe(const char *file)
{
    EndTextureModeSafe();
    return rl::LoadTexture(file);
}

rl::Texture2D rg::LoadTextureFromImageSafe(const rl::Image &image)
{
    EndTextureModeSafe();
    return LoadTextureFromImage(image);
}

rl::Image rg::LoadImageFromTextureSafe(const rl::Texture &texture)
{
    EndTextureModeSafe();
    return LoadImageFromTexture(texture);
}

void rg::UnloadTextureSafe(const rl::Texture2D &texture)
{
    EndTextureModeSafe();
    UnloadTexture(texture);
}

rl::RenderTexture2D rg::LoadRenderTextureSafe(const int width, const int height)
{
    EndTextureModeSafe();
    return rl::LoadRenderTexture(width, height);
}

void rg::UnloadRenderTextureSafe(const rl::RenderTexture2D &render)
{
    EndTextureModeSafe();
    UnloadRenderTexture(render);
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

std::uniform_real_distribution<float>
rg::math::random_uniform_dist(const float min, const float max)
{
    // ReSharper disable once CppTemplateArgumentsCanBeDeduced - it deduces to double, not float
    const std::uniform_real_distribution<float> dist(min, max);
    return dist;
}

float rg::math::random_uniform(std::uniform_real_distribution<float> dist)
{
    return dist(gen);
}

float rg::math::get_random_uniform(const float min, const float max)
{
    if (dists.find(min) == dists.end())
    {
        dists[min] = {};
    }
    if (dists[min].find(max) == dists[min].end())
    {
        dists[min][max] = random_uniform_dist(min, max);
    }

    return dists[min][max](gen);
}

float rg::math::clamp(const float value, const float min, const float max)
{
    return rl::Clamp(value, min, max);
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

rg::Rect rg::Rect::move(const math::Vector2 delta)
{
    x += delta.x;
    y += delta.y;
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

bool rg::Rect::collideline(
        Line line, math::Vector2 *collisionPoint1, math::Vector2 *collisionPoint2) const
{
    bool hasCollision = false;
    bool secondCollision = false;
    math::Vector2 *collisionPoint = collisionPoint1;

    const Line lineTop{topleft(), topright()};
    const Line lineBottom{bottomleft(), bottomright()};
    const Line lineLeft{topleft(), bottomleft()};
    const Line lineRight{topright(), bottomright()};

    if (line.collideline(lineTop, collisionPoint))
    {
        collisionPoint = collisionPoint2;
        hasCollision = true;
    }
    if (line.collideline(lineBottom, collisionPoint))
    {
        if (hasCollision)
        {
            secondCollision = true;
        }
        else
        {
            collisionPoint = collisionPoint2;
        }
        hasCollision = true;
    }
    if (!secondCollision && line.collideline(lineLeft, collisionPoint))
    {
        if (hasCollision)
        {
            secondCollision = true;
        }
        else
        {
            collisionPoint = collisionPoint2;
        }
        hasCollision = true;
    }
    if (!secondCollision && line.collideline(lineRight, collisionPoint))
    {
        hasCollision = true;
    }

    return hasCollision;
}

bool rg::Rect::colliderect(const Rect &other) const
{
    return CheckCollisionRecs(rectangle, other.rectangle);
}

rg::Line rg::Rect::clipline(const Line line)
{
    return clipline(line.start, line.end);
}

rg::Line rg::Rect::clipline(const math::Vector2 start, const math::Vector2 end)
{
    return clipline(start.x, start.y, end.x, end.y);
}

/* For use with the Cohen-Sutherland algorithm for line clipping, in SDL_rect_impl.h */
#define CODE_BOTTOM 1
#define CODE_TOP 2
#define CODE_LEFT 4
#define CODE_RIGHT 8

/* Use the Cohen-Sutherland algorithm for line clipping */
static int COMPUTEOUTCODE(const rg::Rect *rect, const float x, const float y)
{
    int code = 0;
    if (y < rect->y)
    {
        code |= CODE_TOP;
    }
    else if (y >= rect->y + rect->height)
    {
        code |= CODE_BOTTOM;
    }
    if (x < rect->x)
    {
        code |= CODE_LEFT;
    }
    else if (x >= rect->x + rect->width)
    {
        code |= CODE_RIGHT;
    }
    return code;
}

// SDL_INTERSECTRECTANDLINE
rg::Line rg::Rect::clipline(float x1, float y1, float x2, float y2)
{
    Line result{};

    float rectx1 = x;
    float recty1 = y;
    float rectx2 = x + width - 1;
    float recty2 = y + height - 1;
    int outcode1, outcode2;

    /* Check to see if entire line is inside rect */
    if (x1 >= rectx1 && x1 <= rectx2 && x2 >= rectx1 && x2 <= rectx2 && y1 >= recty1 &&
        y1 <= recty2 && y2 >= recty1 && y2 <= recty2)
    {
        result = {x1, y1, x2, y2};
        return result;
    }

    /* Check to see if entire line is to one side of rect */
    if ((x1 < rectx1 && x2 < rectx1) || (x1 > rectx2 && x2 > rectx2) ||
        (y1 < recty1 && y2 < recty1) || (y1 > recty2 && y2 > recty2))
    {
        return result;
    }

    if (y1 == y2)
    { /* Horizontal line, easy to clip */
        if (x1 < rectx1)
        {
            result.x1 = rectx1;
        }
        else if (x1 > rectx2)
        {
            result.x1 = rectx2;
        }
        if (x2 < rectx1)
        {
            result.x2 = rectx1;
        }
        else if (x2 > rectx2)
        {
            result.x2 = rectx2;
        }
        return result;
    }

    if (x1 == x2)
    { /* Vertical line, easy to clip */
        if (y1 < recty1)
        {
            result.y1 = recty1;
        }
        else if (y1 > recty2)
        {
            result.y1 = recty2;
        }
        if (y2 < recty1)
        {
            result.y2 = recty1;
        }
        else if (y2 > recty2)
        {
            result.y2 = recty2;
        }
        return result;
    }

    /* More complicated Cohen-Sutherland algorithm */
    outcode1 = COMPUTEOUTCODE(this, x1, y1);
    outcode2 = COMPUTEOUTCODE(this, x2, y2);
    while (outcode1 || outcode2)
    {
        if (outcode1 & outcode2)
        {
            return result;
        }

        if (outcode1)
        {
            if (outcode1 & CODE_TOP)
            {
                y = recty1;
                x = (x1 + ((x2 - x1) * (y - y1)) / (y2 - y1));
            }
            else if (outcode1 & CODE_BOTTOM)
            {
                y = recty2;
                x = (x1 + ((x2 - x1) * (y - y1)) / (y2 - y1));
            }
            else if (outcode1 & CODE_LEFT)
            {
                x = rectx1;
                y = (y1 + ((y2 - y1) * (x - x1)) / (x2 - x1));
            }
            else if (outcode1 & CODE_RIGHT)
            {
                x = rectx2;
                y = (y1 + ((y2 - y1) * (x - x1)) / (x2 - x1));
            }
            x1 = x;
            y1 = y;
            outcode1 = COMPUTEOUTCODE(this, x, y);
        }
        else
        {
            if (outcode2 & CODE_TOP)
            {
                assert(y2 != y1); /* if equal: division by zero. */
                y = recty1;
                x = x1 + (x2 - x1) * (y - y1) / (y2 - y1);
            }
            else if (outcode2 & CODE_BOTTOM)
            {
                assert(y2 != y1); /* if equal: division by zero. */
                y = recty2;
                x = x1 + (x2 - x1) * (y - y1) / (y2 - y1);
            }
            else if (outcode2 & CODE_LEFT)
            {
                /* If this assertion ever fires, here's the static analysis that warned about it:
                   http://buildbot.libsdl.org/sdl-static-analysis/sdl-macosx-static-analysis/sdl-macosx-static-analysis-1101/report-b0d01a.html#EndPath
                 */
                assert(x2 != x1); /* if equal: division by zero. */
                x = rectx1;
                y = y1 + (y2 - y1) * (x - x1) / (x2 - x1);
            }
            else if (outcode2 & CODE_RIGHT)
            {
                /* If this assertion ever fires, here's the static analysis that warned about it:
                   http://buildbot.libsdl.org/sdl-static-analysis/sdl-macosx-static-analysis/sdl-macosx-static-analysis-1101/report-39b114.html#EndPath
                 */
                assert(x2 != x1); /* if equal: division by zero. */
                x = rectx2;
                y = y1 + (y2 - y1) * (x - x1) / (x2 - x1);
            }
            x2 = x;
            y2 = y;
            outcode2 = COMPUTEOUTCODE(this, x, y);
        }
    }
    result.x1 = x1;
    result.y1 = y1;
    result.x2 = x2;
    result.y2 = y2;

    return result;
}

bool rg::Line::collidepoint(const math::Vector2 point, const float threshold) const
{
    return CheckCollisionPointLine(point.vector2, start.vector2, end.vector2, threshold);
}

bool rg::Line::collideline(const Line other, math::Vector2 *collisionPoint) const
{
    return CheckCollisionLines(
            start.vector2, end.vector2, other.start.vector2, other.end.vector2,
            &collisionPoint->vector2);
}

rg::Line::operator bool() const
{
    return x1 != x2 || y1 != y2;
}

void rg::Surface::Setup(const int width, const int height)
{
    if (!render.id)
    {
        render = LoadRenderTextureSafe(width, height);
    }

    // RenderTexture draws textures upside-down
    atlas_rect = {0, 0, (float) width, (float) height};

    Fill(rl::BLACK);
}

rg::Surface::Surface(const int width, const int height)
{
    Setup(width, height);
}

rg::Surface::Surface(const math::Vector2 size)
{
    Setup(size.x, size.y);
}

rg::Surface::Surface(rl::Texture2D *texture, const Rect atlas)
    : atlas_rect(atlas), shared_texture(texture)
{
    if (!atlas_rect.width)
    {
        atlas_rect.width = texture->width;
    }
    if (!atlas_rect.height)
    {
        atlas_rect.height = texture->height;
    }
}

rg::Surface::~Surface()
{
    if (render.id && !parent)
    {
        UnloadRenderTextureSafe(render);
        render.id = 0;
    }
}

void rg::Surface::Fill(const rl::Color color)
{
    TraceLog(
            rl::LOG_TRACE,
            rl::TextFormat("Fill render %d texture %d", render.id, render.texture.id));
    ToggleRender();
    ClearBackground(color);
}

void rg::Surface::Blit(
        const std::shared_ptr<Surface> &incoming, const math::Vector2 offset,
        const rl::BlendMode blend_mode)
{
    TraceLog(
            rl::LOG_TRACE, "Blit render %d texture %d Texture() %d into render %d texture %d",
            incoming->render.id, incoming->render.texture.id, incoming->GetTexture().id, render.id,
            render.texture.id);
    this->Blit(
            incoming->GetTexture(), offset,
            {incoming->atlas_rect.x, incoming->atlas_rect.y, incoming->atlas_rect.width,
             incoming->atlas_rect.height * incoming->flip_atlas_height},
            blend_mode, incoming->tint);
}

void rg::Surface::Blits(
        const std::vector<std::pair<std::shared_ptr<Surface>, math::Vector2>> &blit_sequence,
        const rl::BlendMode blend_mode)
{
    if (blit_sequence.empty())
    {
        return;
    }
    TraceLog(rl::LOG_DEBUG, rl::TextFormat("Blits %d sequences", blit_sequence.size()));

    ToggleRender();

    // draw incoming as blended
    if (blend_mode != rl::BLEND_ALPHA)
    {
        BeginBlendMode(blend_mode);
    }
    for (auto &[surface, offset]: blit_sequence)
    {
        DrawTextureRec(
                surface->GetTexture(),
                {surface->atlas_rect.x, surface->atlas_rect.y, surface->atlas_rect.width,
                 -surface->atlas_rect.height * surface->flip_atlas_height},
                offset.vector2, surface->tint);
    }
    if (blend_mode != rl::BLEND_ALPHA)
    {
        rl::EndBlendMode();
    }
}

void rg::Surface::Blit(
        const rl::Texture2D &incoming_texture, const math::Vector2 offset, const Rect area,
        const rl::BlendMode blend_mode, const rl::Color tint)
{
    if (!incoming_texture.id)
    {
        return;
    }
    TraceLog(
            rl::LOG_TRACE, "Blit texture %d into render %d texture %d", incoming_texture.id,
            render.id, render.texture.id);

    ToggleRender();

    // draw incoming as blended
    if (blend_mode != rl::BLEND_ALPHA)
    {
        BeginBlendMode(blend_mode);
    }
    if (area.height && area.width)
    {
        DrawTextureRec(
                incoming_texture, {area.x, area.y, area.width, -area.height}, offset.vector2, tint);
    }
    else
    {
        DrawTextureRec(
                incoming_texture,
                {0, 0, (float) incoming_texture.width, (float) -incoming_texture.height},
                offset.vector2, tint);
    }
    if (blend_mode != rl::BLEND_ALPHA)
    {
        rl::EndBlendMode();
    }
}

rg::Rect rg::Surface::GetRect() const
{
    const float absWidth = atlas_rect.width > 0 ? atlas_rect.width : -atlas_rect.width;
    const float absHeight = atlas_rect.height > 0 ? atlas_rect.height : -atlas_rect.height;
    return {0, 0, absWidth, absHeight};
}

std::shared_ptr<rg::Surface> rg::Surface::SubSurface(const Rect rect)
{
    auto result = std::make_shared<Surface>(GetTexture().width, GetTexture().height);
    UnloadRenderTextureSafe(result->render);
    result->render = render;
    result->shared_texture = shared_texture;
    result->atlas_rect = rect;
    result->parent = shared_from_this();
    result->offset = rect.pos;
    return result;
}

std::shared_ptr<rg::Surface> rg::Surface::GetParent()
{
    return parent;
}

std::shared_ptr<rg::Surface> rg::Surface::GetAbsParent()
{
    std::shared_ptr<Surface> result = shared_from_this();
    while (result->parent)
    {
        result = result->parent;
    }
    return result;
}

rl::Texture2D rg::Surface::GetTexture() const
{
    if (shared_texture)
    {
        return *shared_texture;
    }
    return render.texture;
}

void rg::Surface::ToggleRender()
{
    if (current_render != render.id)
    {
        EndTextureModeSafe();
        TraceLog(
                rl::LOG_TRACE,
                rl::TextFormat("Begin render %d texture %d", render.id, render.texture.id));
        BeginTextureModeSafe(render);
        shared_texture = nullptr;
    }
}

void rg::Surface::SetColorKey(const rl::Color color)
{
    TraceLog(
            rl::LOG_TRACE,
            rl::TextFormat("SetColorKey render %d texture %d", render.id, render.texture.id));
    rl::Image current = LoadImageFromTextureSafe(GetTexture());
    ImageColorReplace(&current, color, rl::BLANK);
    const rl::Texture color_texture = LoadTextureFromImageSafe(current);

    // replace
    Fill(rl::BLANK);
    Blit(color_texture, {}, atlas_rect);

    // clean up
    UnloadTextureSafe(color_texture);
    UnloadImage(current);
}

void rg::Surface::SetAlpha(const float alpha)
{
    tint.r = alpha;
    tint.g = alpha;
    tint.b = alpha;
    tint.a = alpha;
}

void rg::Surface::Blit(
        const std::shared_ptr<Surface> &incoming, Rect offset, rl::BlendMode blend_mode)
{
    Blit(incoming, offset.pos, blend_mode);
}

std::shared_ptr<rg::Surface> rg::Surface::convert(const rl::PixelFormat format) const
{
    const auto result = std::make_shared<Surface>(GetTexture().width, GetTexture().height);

    rl::Image toConvert = LoadImageFromTextureSafe(GetTexture());
    ImageFormat(&toConvert, format);

    const rl::Texture2D converted = LoadTextureFromImageSafe(toConvert);
    result->Blit(converted, {}, {});

    UnloadTextureSafe(converted);
    UnloadImage(toConvert);
    return result;
}

std::shared_ptr<rg::Surface> rg::image::Load(const char *path)
{
    // we Blit the loaded texture so it is considered local and unloaded in ~Surface()
    const rl::Texture2D loaded_texture = LoadTextureSafe(path);
    const auto surface = std::make_shared<Surface>(loaded_texture.width, loaded_texture.height);
    surface->Fill(rl::BLANK);
    surface->Blit(
            loaded_texture, {},
            {0, 0, (float) loaded_texture.width, -(float) loaded_texture.height});
    UnloadTextureSafe(loaded_texture);
    return surface;
}

std::vector<std::shared_ptr<rg::Surface>> rg::image::LoadFolderList(const char *path)
{
    std::vector<std::shared_ptr<Surface>> surfaces;
    for (const auto &dirEntry: std::filesystem::recursive_directory_iterator(path))
    {
        auto entryPath = dirEntry.path().string();
        surfaces.push_back(Load(entryPath.c_str()));
    }
    return surfaces;
}

std::map<std::string, std::shared_ptr<rg::Surface>> rg::image::LoadFolderDict(const char *path)
{
    std::map<std::string, std::shared_ptr<Surface>> surfaces;
    for (const auto &dirEntry: std::filesystem::recursive_directory_iterator(path))
    {
        auto filename = dirEntry.path().stem().string();
        auto entryPath = dirEntry.path().string();
        // ReSharper disable once CppDFAMemoryLeak
        surfaces[filename] = Load(entryPath.c_str());
    }
    return surfaces;
}

std::vector<std::shared_ptr<rg::Surface>> rg::image::ImportFolder(const char *path)
{
    std::vector<std::shared_ptr<Surface>> surfaces;
    for (const auto &dirEntry: std::filesystem::recursive_directory_iterator(path))
    {
        auto entryPath = dirEntry.path().string();
        surfaces.push_back(Load(entryPath.c_str()));
    }
    return surfaces;
}

std::map<std::string, std::shared_ptr<rg::Surface>> rg::image::ImportFolderDict(const char *path)
{
    std::map<std::string, std::shared_ptr<Surface>> result;
    for (const auto &dirEntry: std::filesystem::recursive_directory_iterator(path))
    {
        auto entryPath = dirEntry.path().string();
        auto filename = dirEntry.path().stem().string();
        result[filename] = Load(entryPath.c_str());
    }
    return result;
}

rg::Frames::Frames(const int width, const int height, int rows, int cols)
    : Surface(width, height), rows(rows), cols(cols)
{
    CreateFrames(width, height, rows, cols);
    atlas_rect = frames[current_frame_index];
    flip_atlas_height = -1;
}


rg::Frames::Frames(const std::shared_ptr<Surface> &surface, const int rows, const int cols)
    : Frames(surface->GetRect().width, surface->GetRect().height, rows, cols)
{
    Blit(surface, math::Vector2{});
    flip_atlas_height = -1;
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

void rg::Frames::SetAtlas(const int frame_index)
{
    current_frame_index = (frame_index % (int) frames.size() + frames.size()) % frames.size();
    atlas_rect = frames[current_frame_index];
}

std::shared_ptr<rg::Frames> rg::Frames::Merge(
        const std::vector<std::shared_ptr<Surface>> &surfaces, const int rows, const int cols)
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

std::shared_ptr<rg::Frames> rg::Frames::Load(const char *file, int rows, int cols)
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

std::shared_ptr<rg::Frames> rg::Frames::SubFrames(const Rect rect)
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

void rg::draw::rect(
        const std::shared_ptr<Surface> &surface, const rl::Color color, const Rect rect,
        const float lineThick, const float radius, const bool topLeft, const bool topRight,
        const bool bottomLeft, const bool bottomRight)
{
    TraceLog(
            rl::LOG_TRACE, rl::TextFormat(
                                   "draw::rect render %d texture %d", surface->render.id,
                                   surface->render.texture.id));
    surface->ToggleRender();
    if (lineThick > 0)
    {
        if (radius > 0)
        {
            const float greater_dim = (rect.width < rect.height ? rect.width : rect.height) * 0.5f;
            const float r = (radius > greater_dim) ? greater_dim : radius;
            const float roundness = r / greater_dim;
            const int segments = roundness * 90;
            DrawRectangleRoundedLinesEx(rect.rectangle, roundness, segments, lineThick, color);
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
            const float greater_dim = (rect.width < rect.height ? rect.width : rect.height) * 0.5f;
            const float r = (radius > greater_dim) ? greater_dim : radius;
            const float roundness = r / greater_dim;
            const int segments = roundness * 90;
            DrawRectangleRounded(rect.rectangle, roundness, segments, color);
            Rect corner = {0, 0, radius, radius};
            if (!topLeft)
            {
                corner.topleft(rect.topleft());
                DrawRectangleRec(corner.rectangle, color);
            }
            if (!topRight)
            {
                corner.topright(rect.topright());
                DrawRectangleRec(corner.rectangle, color);
            }
            if (!bottomLeft)
            {
                corner.bottomleft(rect.bottomleft());
                DrawRectangleRec(corner.rectangle, color);
            }
            if (!bottomRight)
            {
                corner.bottomright(rect.bottomright());
                DrawRectangleRec(corner.rectangle, color);
            }
        }
        else
        {
            DrawRectangleV(rect.pos.vector2, rect.size.vector2, color);
        }
    }
}

void rg::draw::circle(
        const std::shared_ptr<Surface> &surface, const rl::Color color, const math::Vector2 center,
        const float radius, const float lineThick)
{
    TraceLog(
            rl::LOG_TRACE, rl::TextFormat(
                                   "draw::circle render %d texture %d", surface->render.id,
                                   surface->render.texture.id));
    surface->ToggleRender();

    if (lineThick > 0)
    {
        DrawCircleLinesV(center.vector2, radius, color);
    }
    else if (lineThick == 0)
    {
        DrawCircleV(center.vector2, radius, color);
    }
}

void rg::draw::bar(
        const std::shared_ptr<Surface> &surface, const Rect rect, const float value,
        const float max_value, const rl::Color color, const rl::Color bg_color, const float radius)
{
    TraceLog(
            rl::LOG_TRACE, rl::TextFormat(
                                   "draw::bar render %d texture %d", surface->render.id,
                                   surface->render.texture.id));
    const float ratio = rect.width / max_value;
    const Rect progress_rect = {
            rect.x, rect.y, math::clamp(value * ratio, 0, rect.width), rect.height};

    if (radius == 0)
    {
        draw::rect(surface, bg_color, rect);
        draw::rect(surface, color, progress_rect);
    }
    else
    {
        draw::rect(surface, bg_color, rect, 0, radius);
        draw::rect(surface, color, progress_rect, 0, radius);
    }
}

void rg::draw::line(
        const std::shared_ptr<Surface> &surface, const rl::Color color, const math::Vector2 start,
        const math::Vector2 end, const float width)
{
    TraceLog(
            rl::LOG_TRACE, rl::TextFormat(
                                   "draw::line render %d texture %d", surface->render.id,
                                   surface->render.texture.id));
    surface->ToggleRender();

    if (width > 1)
    {
        DrawLineEx(start.vector2, end.vector2, width, color);
    }
    else if (width == 1)
    {
        DrawLineV(start.vector2, end.vector2, color);
    }
}

void rg::draw::lines(
        const std::shared_ptr<Surface> &surface, rl::Color color, bool closed,
        std::vector<math::Vector2> points, float width)
{
    TraceLog(
            rl::LOG_TRACE, rl::TextFormat(
                                   "draw::lines render %d texture %d", surface->render.id,
                                   surface->render.texture.id));
    surface->ToggleRender();

    int pointCount = points.size();
    if (closed)
    {
        pointCount += 1;
    }

    auto *pts = new rl::Vector2[pointCount];
    for (unsigned int i = 0; i < points.size(); ++i)
    {
        pts[i] = points[i].vector2;
    }
    if (closed)
    {
        pts[pointCount - 1] = points[0].vector2;
    }

    DrawSplineLinear(pts, pointCount, width, color);

    delete[] pts;
}

#ifdef WITH_TMX
rl::Texture2D *rg::tmx::GetTMXTileTexture(const rl::tmx_tile *tile, Rect *atlas_rect)
{
    const rl::tmx_image *im = tile->image;
    rl::Texture2D *map_texture = nullptr;

    atlas_rect->x = tile->ul_x;
    atlas_rect->y = tile->ul_y;
    atlas_rect->width = tile->width;
    atlas_rect->height = -(float) tile->height;

    if (im && im->resource_image)
    {
        map_texture = (rl::Texture2D *) im->resource_image;
    }
    else if (tile->tileset->image->resource_image)
    {
        map_texture = (rl::Texture2D *) tile->tileset->image->resource_image;
    }

    return map_texture;
}

std::vector<rg::tmx::TileInfo>
rg::tmx::GetTMXTiles(const rl::tmx_map *map, const rl::tmx_layer *layer)
{
    std::vector<TileInfo> tiles{};
    tiles.reserve(map->height * map->width);
    for (unsigned int y = 0; y < map->height; y++)
    {
        for (unsigned int x = 0; x < map->width; x++)
        {
            const unsigned int baseGid = layer->content.gids[y * map->width + x];
            const unsigned int gid = baseGid & TMX_FLIP_BITS_REMOVAL;
            if (map->tiles[gid])
            {
                const rl::tmx_tileset *ts = map->tiles[gid]->tileset;
                Rect atlas_rect{};
                auto *tileTexture = GetTMXTileTexture(map->tiles[gid], &atlas_rect);
                const math::Vector2 pos = {(float) x * ts->tile_width, (float) y * ts->tile_height};
                TileInfo tile_info = {pos, tileTexture, atlas_rect};
                tiles.push_back(tile_info);
            }
        }
    }
    return tiles;
}

std::shared_ptr<rg::Surface>
rg::tmx::GetTMXLayerSurface(const rl::tmx_map *map, const rl::tmx_layer *layer)
{
    const auto surface = std::make_shared<Surface>(
            (int) (map->width * map->tile_width), (int) (map->height * map->tile_height));
    surface->Fill(rl::BLANK);
    // GetTMXTiles will return many Texture*, but we don't need to unload them here, only
    // at rg::UnloadTMX
    const std::vector<TileInfo> tiles = GetTMXTiles(map, layer);
    for (const auto &[position, texture, atlas_rect]: tiles)
    {
        surface->Blit(*texture, position, atlas_rect);
    }
    return surface;
}

rg::math::Vector2 rg::tmx::GetTMXObjPosition(const rl::tmx_object *object)
{
    float x = 0, y = 0;
    switch (object->obj_type)
    {
        case rl::OT_NONE:
        case rl::OT_POLYGON:
        case rl::OT_POLYLINE:
        case rl::OT_ELLIPSE:
        case rl::OT_TEXT:
            break;
        case rl::OT_SQUARE:
        case rl::OT_POINT:
            x = object->x;
            y = object->y;
            break;
        case rl::OT_TILE:
            x = object->x;
            y = object->y - object->height;
            break;
    }
    return math::Vector2{x, y};
}

std::map<std::string, rl::tmx_map *> rg::tmx::LoadTMXMaps(const char *path)
{
    std::map<std::string, rl::tmx_map *> result;
    for (const auto &dirEntry: std::filesystem::recursive_directory_iterator(path))
    {
        auto filename = dirEntry.path().stem().string();
        auto entryPath = dirEntry.path().string();
        result[filename] = rl::LoadTMX(entryPath.c_str());
    }
    return result;
}

#endif // WITH_TMX

void rg::sprite::Group::Draw(const std::shared_ptr<Surface> &surface)
{
    for (const auto &sprite: sprites)
    {
        surface->Blit(sprite->image, sprite->rect);
    }
}

void rg::sprite::Group::Update(const float deltaTime) const
{
    for (const auto &sprite: Sprites())
    {
        sprite->Update(deltaTime);
    }
}

void rg::sprite::Group::empty()
{
    for (const auto &sprite: Sprites())
    {
        sprite->remove(this);
    }
    sprites.clear();
}

void rg::sprite::Group::remove(const std::vector<std::shared_ptr<Sprite>> &to_remove_sprites)
{
    for (const auto &sprite: to_remove_sprites)
    {
        remove(sprite);
    }
}

void rg::sprite::Group::remove( // NOLINT(*-no-recursion) - the recursion is broken with has()
        const std::shared_ptr<Sprite> &to_remove_sprite)
{
    if (has(to_remove_sprite))
    {
        sprites.erase(std::remove(sprites.begin(), sprites.end(), to_remove_sprite), sprites.end());
        to_remove_sprite->remove(this);
    }
}

void rg::sprite::Group::add(const std::vector<std::shared_ptr<Sprite>> &to_add_sprites)
{
    for (const auto &sprite: to_add_sprites)
    {
        add(sprite);
    }
}

void rg::sprite::Group::add( // NOLINT(*-no-recursion) - the recursion is broken with has()
        const std::shared_ptr<Sprite> &to_add_sprite)
{
    if (!has(to_add_sprite))
    {
        sprites.push_back(to_add_sprite);
        to_add_sprite->add(this);
    }
}

bool rg::sprite::Group::has(const std::vector<std::shared_ptr<Sprite>> &check_sprites)
{
    for (const auto &sprite: check_sprites)
    {
        if (!has(sprite))
        {
            return false;
        }
    }
    return true;
}

bool rg::sprite::Group::has(const std::shared_ptr<Sprite> &check_sprite)
{
    return std::find(sprites.begin(), sprites.end(), check_sprite) != sprites.end();
}

std::vector<std::shared_ptr<rg::sprite::Sprite>> rg::sprite::Group::Sprites() const
{
    return sprites;
}

rg::sprite::Sprite::Sprite() = default;

// !!!!! Can't have these constructors because it can't call "shared_from_this()" before
// object has actually been created
// rg::sprite::Sprite::Sprite(Group *to_add_group)
// {
//     if (to_add_group)
//     {
//         add(to_add_group);
//     }
// }
//
// rg::sprite::Sprite::Sprite(const std::vector<Group *> &groups)
// {
//     add(groups);
// }

void rg::sprite::Sprite::add( // NOLINT(*-no-recursion) - the recursion is broken with has()
        Group *to_add_group)
{
    if (to_add_group)
    {
        if (!has(to_add_group))
        {
            groups.push_back(to_add_group);
            // to_add_group->add(this);
            to_add_group->add(shared_from_this());
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
        to_remove_group->remove(shared_from_this());
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
            group->remove(shared_from_this());
        }
    }
}

void rg::sprite::Sprite::LeaveAllGroups() // NOLINT(*-no-recursion) - the recursion does not
                                          // happen because we pass `false`
{
    // leave all groups
    for (const auto group: Groups())
    {
        group->remove(shared_from_this());
    }
    // it doesn't belong to any group
    groups.clear();
}

std::shared_ptr<rg::sprite::Sprite> rg::sprite::Sprite::Kill()
{
    // leave all groups
    LeaveAllGroups();
    return shared_from_this();
}

bool rg::sprite::collide_rect(
        const std::shared_ptr<Sprite> &left, const std::shared_ptr<Sprite> &right)
{
    return CheckCollisionRecs(left->rect.rectangle, right->rect.rectangle);
}

std::vector<std::shared_ptr<rg::sprite::Sprite>> rg::sprite::spritecollide(
        const std::shared_ptr<Sprite> &sprite, const Group *group, const bool dokill,
        const std::function<bool(std::shared_ptr<Sprite> left, std::shared_ptr<Sprite> right)>
                &collided)
{
    std::vector<std::shared_ptr<Sprite>> result;
    for (const auto &other_sprite: group->Sprites())
    {
        if (collided(sprite, other_sprite))
        {
            result.push_back(other_sprite);
            if (dokill)
            {
                // just remove from group, don't delete
                // it will be returned in the result
                // if needed, delete it in the vector later
                other_sprite->Kill();
            }
        }
    }
    return result;
}

std::shared_ptr<rg::sprite::Sprite> rg::sprite::spritecollideany(
        const std::shared_ptr<Sprite> &sprite, const Group *group,
        const std::function<bool(std::shared_ptr<Sprite> left, std::shared_ptr<Sprite> right)>
                &collided)
{
    for (auto other_sprite: group->Sprites())
    {
        if (collided(sprite, other_sprite))
        {
            return other_sprite;
        }
    }
    return nullptr;
}

rg::sprite::collide_rect_ratio::collide_rect_ratio(const float ratio) : ratio(ratio)
{}

bool rg::sprite::collide_rect_ratio::operator()(
        const std::shared_ptr<Sprite> left, const std::shared_ptr<Sprite> right) const
{
    Rect leftrect = left->rect;
    Rect rightrect = right->rect;

    leftrect.scale_by_ip(ratio);
    rightrect.scale_by_ip(ratio);

    return collide_rect(left, right);
}

// duration is in seconds
rg::Timer::Timer(
        const float duration, const bool repeat, const bool autostart,
        const std::function<void()> &func)
    : duration(duration), repeat(repeat), autostart(autostart), func(func)
{
    if (this->autostart)
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
        *(rl::Music *) audio = rl::LoadMusicStream(file);
        musics.push_back(this);
    }
    else
    {
        audio = new rl::Sound;
        *(rl::Sound *) audio = rl::LoadSound(file);
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
        if (IsMusicStreamPlaying(*(rl::Music *) audio))
        {
            StopMusicStream(*(rl::Music *) audio);
        }
    }
    else
    {
        if (IsSoundPlaying(*(rl::Sound *) audio))
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

const char *rg::mixer::Sound::GetFilename() const
{
    return file;
}

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

std::shared_ptr<rg::Surface> rg::mask::Mask::ToSurface() const
{
    const rl::Texture2D maskTexture = LoadTextureFromImageSafe(image);
    const auto surface = std::make_shared<Surface>(image.width, image.height);
    surface->Fill(rl::BLANK);
    surface->Blit(maskTexture, {}, atlas_rect);
    UnloadTextureSafe(maskTexture);
    return surface;
}

std::shared_ptr<rg::Frames> rg::mask::Mask::ToFrames(int rows, int cols) const
{
    const rl::Texture2D maskTexture = LoadTextureFromImageSafe(image);
    const auto surface = std::make_shared<Frames>(image.width, image.height, rows, cols);
    surface->Fill(rl::BLANK);
    surface->Blit(maskTexture, {}, atlas_rect);
    surface->SetAtlas();
    UnloadTextureSafe(maskTexture);
    return surface;
}

rg::mask::Mask
rg::mask::FromSurface(const std::shared_ptr<Surface> &surface, const unsigned char threshold)
{
    auto mask = Mask(surface->GetRect().width, surface->GetRect().height);
    const rl::Image surfImage = LoadImageFromTextureSafe(surface->GetTexture());
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
    mask.atlas_rect = surface->atlas_rect;

    UnloadImage(alphaImage);
    UnloadImage(surfImage);
    return mask;
}

rg::mask::Mask
rg::mask::FromSurface(const std::shared_ptr<Frames> &frames, const unsigned char threshold)
{
    auto mask = Mask(frames->render.texture.width, frames->render.texture.height);
    const rl::Image surfImage = LoadImageFromTextureSafe(frames->render.texture);
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
    mask.atlas_rect =
            Rect{0, 0, (float) frames->render.texture.width, (float) frames->render.texture.height};

    UnloadImage(alphaImage);
    UnloadImage(surfImage);
    return mask;
}

rg::font::Font::Font(const float font_size) : font(rl::GetFontDefault()), font_size(font_size)
{}

rg::font::Font::Font(const char *file, const float font_size)
    : font(rl::LoadFontEx(file, font_size, nullptr, 0)), font_size(font_size)
{}

// rl:Font is trivial copiable
// ReSharper disable once CppPassValueParameterByConstReference
rg::font::Font::Font(rl::Font font, const float font_size) : font(font), font_size(font_size)
{}

rg::font::Font::~Font()
{
    UnloadFont(font);
}

std::shared_ptr<rg::Surface> rg::font::Font::render(
        const char *text, const rl::Color color, const float spacing, const rl::Color bg,
        const float padding_width, const float padding_height) const
{
    TraceLog(rl::LOG_TRACE, rl::TextFormat("Font::render %s", text));
    const rl::Image imageText = ImageTextEx(font, text, font_size, spacing, color);
    const rl::Texture texture = LoadTextureFromImageSafe(imageText);

    const int surfWidth = imageText.width + padding_width;
    const int surfHeight = imageText.height + padding_height;

    const auto result = std::make_shared<Surface>(surfWidth, surfHeight);
    result->Fill(bg);
    result->Blit(
            texture, {padding_width / 2.0f, padding_height / 2.0f},
            {0, 0, (float) texture.width, -(float) texture.height});

    UnloadTextureSafe(texture);
    UnloadImage(imageText);
    return result;
}

rg::math::Vector2 rg::font::Font::size(const char *text) const
{
    return {MeasureTextEx(font, text, font_size, 1)};
}

std::shared_ptr<rg::Surface>
rg::transform::Flip(const std::shared_ptr<Surface> &surface, const bool flip_x, const bool flip_y)
{
    const auto result =
            std::make_shared<Surface>(surface->GetRect().width, surface->GetRect().height);
    result->Fill(rl::BLANK);
    result->Blit(surface->GetTexture(), {});
    if (flip_x)
    {
        result->atlas_rect.width = -result->atlas_rect.width;
    }
    if (flip_y)
    {
        result->atlas_rect.height = -result->atlas_rect.height;
    }

    return result;
}

std::shared_ptr<rg::Frames>
rg::transform::Flip(const std::shared_ptr<Frames> &frames, const bool flip_x, const bool flip_y)
{
    const auto result = std::make_shared<Frames>(
            (float) frames->render.texture.width, (float) frames->render.texture.height,
            frames->rows, frames->cols);
    result->frames = frames->frames;
    result->Fill(rl::BLANK);
    result->Blit(frames->render.texture, {});
    if (flip_x)
    {
        for (auto &frame: result->frames)
        {
            frame.width = -frame.width;
        }
    }
    if (flip_y)
    {
        for (auto &frame: result->frames)
        {
            frame.height = -frame.height;
        }
    }
    result->SetAtlas();

    return result;
}

rg::math::Vector2 operator+(const rg::math::Vector2 &lhs, const rg::math::Vector2 &rhs)
{
    return {lhs.x + rhs.x, lhs.y + rhs.y};
}

rg::math::Vector2 operator-(const rg::math::Vector2 &lhs, const rg::math::Vector2 &rhs)
{
    return {lhs.x - rhs.x, lhs.y - rhs.y};
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

rg::math::Vector2 operator*(const rg::math::Vector2 &lhs, const float scale)
{
    return {lhs.x * scale, lhs.y * scale};
}

rg::math::Vector2 &operator*=(rg::math::Vector2 &lhs, const float scale)
{
    lhs.x *= scale;
    lhs.y *= scale;
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

float rg::math::Vector2::distance_to(const Vector2 target) const
{
    return Vector2Distance(vector2, target.vector2);
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

rg::math::Vector2::operator bool() const
{
    return x || y;
}
