#pragma once
#include <functional>
#include <raylib.h>
#include <vector>

#ifndef MAX_TEXT_BUFFER_LENGTH
#define MAX_TEXT_BUFFER_LENGTH 1024
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

class Surface
{
public:

    Surface(int width, int height);
    ~Surface();
    void Fill(Color color) const;
    void Blit(Surface *surface, Vector2 offset = {0, 0}) const;
    // Returns the size of the Surface, not the atlas position
    [[nodiscard]] RectangleU GetRect() const;
    Texture2D *Texture();

    static Surface *Load(const char *path);

    RectangleU atlas_rect; // atlas position

private:

    RenderTexture2D render_texture; // atlas texture
};

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
    inline Surface *display_surface;

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

    // Returns center of rectangle
    Vector2 GetRectCenter(RectangleU rect);
    // Returns mid-bottom of rectangle
    Vector2 GetRectMidBottom(RectangleU rect);
    // Returns mid-top of rectangle
    Vector2 GetRectMidTop(RectangleU rect);
    // Returns mid-left of rectangle
    Vector2 GetRectMidLeft(RectangleU rect);
    // Returns mid-right of rectangle
    Vector2 GetRectMidRight(RectangleU rect);
    // Returns top-left of rectangle
    Vector2 GetRectTopLeft(RectangleU rect);
    // Returns top-right of rectangle
    Vector2 GetRectTopRight(RectangleU rect);
    // Returns bottom-left of rectangle
    Vector2 GetRectBottomLeft(RectangleU rect);
    // Returns bottom-right of rectangle
    Vector2 GetRectBottomRight(RectangleU rect);
    // move rectangle's center to position
    void RectToCenter(RectangleU &rect, Vector2 pos);
    // move rectangle's mid-bottom to position
    void RectToMidBottom(RectangleU &rect, Vector2 pos);
    // move rectangle's mid-left to position
    void RectToMidLeft(RectangleU &rect, Vector2 pos);
    // move rectangle's bottom-left to position
    void RectToBottomLeft(RectangleU &rect, Vector2 pos);
    // move rectangle's top-left to position
    void RectToTopLeft(RectangleU &rect, Vector2 pos);
    // move rectangle's top-right to position
    void RectToTopRight(RectangleU &rect, Vector2 pos);

    Vector2 operator+(const Vector2 &lhs, const Vector2 &rhs);
    Vector2 &operator+=(Vector2 &lhs, const Vector2 &rhs);
    Vector2 &operator-=(Vector2 &lhs, const Vector2 &rhs);
    Vector2 operator*(const Vector2 &lhs, float scale);

    std::vector<Surface *> ImportFolder(const char *path);

#ifdef __cplusplus
}
#endif

class SimpleSprite;

class SpriteGroup
{
public:

    virtual ~SpriteGroup();
    virtual void Draw(Surface *surface);
    void Update(float deltaTime);
    std::vector<SimpleSprite *> sprites{};
    std::vector<SimpleSprite *> to_delete{};
};

class SimpleSprite
{
public:

    // Pass group by reference because the sprite does not own the group
    explicit SimpleSprite(SpriteGroup &sprite_group);
    // Pass group by reference because the sprite does not own the group
    explicit SimpleSprite(const std::vector<SpriteGroup *> &sprite_groups);
    virtual ~SimpleSprite();

    virtual void Update(float deltaTime){};
    void LeaveOtherGroups(const SpriteGroup *sprite_group);
    virtual void Kill();
    virtual void FlipH();

    unsigned int z = 0; // in 2D games, used to sort the drawing order

    RectangleU rect{}; // world position
    Surface *image = nullptr;
    std::vector<SpriteGroup *> groups{};
};

class Timer
{
public:

    // Default constructor
    Timer() : duration(0.0f), repeat(false), autostart(false), func([]() {})
    {}

    // Parameterized constructor
    explicit
    Timer(float duration, bool repeat = false, bool autostart = false,
          const std::function<void()> &func = std::function<void()>{});
    void Activate();
    void Deactivate();
    void Update();
    bool active{};
    float duration;

private:

    bool repeat;
    bool autostart;
    std::function<void()> func;
    double start_time{};
};
