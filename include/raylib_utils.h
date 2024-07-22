#pragma once
#include <functional>
#include <list>
#include <vector>
#include <raylib.h>
#include <raylib-tmx.h>
#include <string>

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
    void Blit(const Texture2D *texture, Vector2 offset = {0, 0}, RectangleU area = {}) const;
    // Returns the size of the Surface, not the atlas position
    [[nodiscard]] RectangleU GetRect() const;
    Texture2D *Texture();

    static Surface *Load(const char *path);

    RectangleU atlas_rect; // atlas position

private:

    RenderTexture2D render_texture; // atlas texture
};

struct TileInfo
{
    Vector2 position;
    Surface *surface;
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
    // Increase/Decrease size of rect, keeping center position
    void RectInflate(RectangleU &rect, float width, float height);

    Vector2 operator+(const Vector2 &lhs, const Vector2 &rhs);
    Vector2 &operator+=(Vector2 &lhs, const Vector2 &rhs);
    Vector2 &operator-=(Vector2 &lhs, const Vector2 &rhs);
    Vector2 operator*(const Vector2 &lhs, float scale);

    std::vector<Surface *> ImportFolder(const char *path);

    // get the tile image from the tileset
    Surface *GetTMXTileSurface(const tmx_tile *tile);
    // get a vector with tile info (position on the layer and surface image)
    std::vector<TileInfo> GetTMXTiles(const tmx_map *map, const tmx_layer *layer);
    // merges all tiles into one single surface image
    Surface *GetTMXLayerSurface(const tmx_map *map, const tmx_layer *layer);

#ifdef __cplusplus
}
#endif

class SimpleSprite;

// Manages multiple sprites at once
class SpriteGroup
{
public:

    virtual ~SpriteGroup();
    // Draw all sprites into surface
    virtual void Draw(Surface *surface);
    // Updates all sprites
    void Update(float deltaTime);
    // Deletes all sprites, removing them from other groups
    void DeleteAll();

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
    // removes sprite from group and mark for deletion
    virtual void Kill();
    // Flip Horizontally (-width)
    virtual void FlipH();

    unsigned int z = 0; // in 2D games, used to sort the drawing order

    RectangleU rect{}; // world position
    Surface *image = nullptr;
    std::vector<SpriteGroup *> groups{}; // groups that this sprite is in
};

// Remains active for certain duration, can repeat once it is done, can autostart
// and calls a func at the end. Remember to call Update() on every frame
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

// Map like container, but keeps order as it was inserted, not based on `keys` as `std::map`
template<typename K, typename V>
class InsertOrderMap
{
public:

    InsertOrderMap() = default;
    InsertOrderMap(std::initializer_list<std::pair<K, V>> init);

    void insert(const K &key, const V &value);
    V &operator[](const K &key);
    typename std::list<std::pair<K, V>>::iterator begin();
    typename std::list<std::pair<K, V>>::iterator end();

private:

    std::list<std::pair<K, V>> order_;
    std::unordered_map<K, V> map_;
};

// template<> classes must have definitions in .h files
// due to specialization during executable compilation->linking
template<typename K, typename V>
InsertOrderMap<K, V>::InsertOrderMap(const std::initializer_list<std::pair<K, V>> init)
{
    for (auto &[key, value]: init)
    {
        insert(key, value);
    }
}

template<typename K, typename V>
void InsertOrderMap<K, V>::insert(const K &key, const V &value)
{
    if (map_.find(key) == map_.end())
    {
        order_.emplace_back(key, value);
    }
    map_[key] = value;
}

template<typename K, typename V>
V &InsertOrderMap<K, V>::operator[](const K &key)
{
    return map_[key];
}

template<typename K, typename V>
typename std::list<std::pair<K, V>>::iterator InsertOrderMap<K, V>::begin()
{
    return order_.begin();
}

template<typename K, typename V>
typename std::list<std::pair<K, V>>::iterator InsertOrderMap<K, V>::end()
{
    return order_.end();
}

enum Axis
{
    HORIZONTAL = 0,
    VERTICAL
};
