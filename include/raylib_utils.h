#pragma once
#include <raylib.h>

#ifndef MAX_TEXT_BUFFER_LENGTH
#define MAX_TEXT_BUFFER_LENGTH 1024
#endif

#ifdef __cplusplus
extern "C"
{
#endif

    // Warns if there is a render already active
    void BeginTextureModeSafe(const RenderTexture2D &render); // Resets active render
    void EndTextureModeSafe();
    // Starts a render with a Clear color
    void BeginTextureModeC(const RenderTexture2D &render, Color color);
    // Starts drawing with a Clear color
    void BeginDrawingC(Color color);

    // raylib has 4 buffers by default in TextFormat() - to add more, need to recompile raylib.
    // This function receives a buffer created by the application.
    // buffer max size is defined by MAX_TEXT_BUFFER_LENGTH
    void TextFormatSafe(char *buffer, const char *format, ...);


#ifdef __cplusplus
}
#endif
