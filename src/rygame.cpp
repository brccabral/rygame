#include "rygame.hpp"
#include "rygame_cl_Rygame.hpp"


Rygame rygame{};

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
    if (!rl::WindowShouldClose())
    {
        rygame.shouldQuit = true;
    }
}

bool rg::WindowCloseOrQuit()
{
    return rl::WindowShouldClose() || rygame.shouldQuit;
}

void rg::BeginTextureModeSafe(const rl::RenderTexture2D &render)
{
    if (rygame.current_render == render.id)
    {
        return;
    }
    if (rygame.current_render)
    {
        char text[MAX_TEXT_BUFFER_LENGTH];
        TextFormatSafe(
                text, "Double call to BeginTextureMode(), previous id %i new id %i",
                rygame.current_render, render.id);
        TraceLog(rl::LOG_WARNING, text);
        EndTextureModeSafe();
    }
    rygame.current_render = render.id;
    BeginTextureMode(render);
}

void rg::EndTextureModeSafe()
{
    if (rygame.current_render)
    {
        TraceLog(rl::LOG_DEBUG, rl::TextFormat("End render %d", rygame.current_render));
        rl::EndTextureMode();
    }
    rygame.current_render = 0;
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

std::vector<std::string> rg::Split(const std::string &s, const char delim)
{
    std::vector<std::string> result;
    std::stringstream ss(s);
    std::string item;

    while (getline(ss, item, delim))
    {
        result.push_back(item);
    }

    return result;
}
