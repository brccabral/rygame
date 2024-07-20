#pragma once
#include <raylib.h>
#include <vector>

#ifndef MAX_TEXT_BUFFER_LENGTH
#define MAX_TEXT_BUFFER_LENGTH 1024
#endif

#ifdef __cplusplus
extern "C"
{
#endif

    /*
      display_surface
      In your game, init with
          `display_surface = LoadRenderTexture(width, height);
          while (!IsRenderTextureReady(display_surface))
          {}`
     */
    inline RenderTexture2D display_surface;

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

typedef union RectangleU
{
    struct
    {
        Vector2 pos, size;
    };
    struct
    {
        Rectangle rectangle;
    };
    struct
    {
        float x, y, width, height;
    };
} RectangleU;

typedef struct TiledTexture
{
    Texture2D *texture; // the atlas image
    RectangleU rect; // this rect is the rect inside the atlas, not the position of an obj on the map
} TiledTexture;

class SimpleSprite;

class SpriteGroup
{
public:

    virtual ~SpriteGroup();
    virtual void Draw(RenderTexture2D surface) const;
    void Update(double deltaTime);
    std::vector<SimpleSprite *> sprites;
    std::vector<SimpleSprite *> to_delete;
};

class SimpleSprite
{
public:

    explicit SimpleSprite(SpriteGroup *sprite_group);
    explicit SimpleSprite(const std::vector<SpriteGroup *> *sprite_groups);
    virtual ~SimpleSprite();

    virtual void Draw(Vector2 offset) const;
    virtual void Update(double deltaTime){};
    void LeaveOtherGroups(const SpriteGroup *sprite_group);
    virtual void Kill();
    virtual void FlipH();

    RectangleU rect{}; // world position
    TiledTexture image{}; // contains texture atlas, and atlas position
    std::vector<SpriteGroup *> groups;
    bool killed{};
};

class Surface
{
public:

    Surface(int width, int height);
    ~Surface();
    void Fill(Color color) const;
    RectangleU GetRect() const;

private:

    RenderTexture2D render_texture{};
};
