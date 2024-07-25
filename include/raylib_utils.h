// ReSharper disable CppClassCanBeFinal
#pragma once
#include <functional>
#include <list>
#include <vector>
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <cstdarg>
#include <filesystem>

namespace rl
{
#include <raylib.h>
#include <raylib-tmx.h>
#include <raymath.h>
} // namespace rl

rl::Vector2 operator+(const rl::Vector2 &lhs, const rl::Vector2 &rhs);
rl::Vector2 &operator+=(rl::Vector2 &lhs, const rl::Vector2 &rhs);
rl::Vector2 &operator-=(rl::Vector2 &lhs, const rl::Vector2 &rhs);
rl::Vector2 operator*(const rl::Vector2 &lhs, float scale);

namespace rg
{

    void Init(int logLevel = rl::LOG_WARNING, unsigned int config_flags = 0, rl::TraceLogCallback callback = nullptr);
    void Quit();

#ifndef MAX_TEXT_BUFFER_LENGTH
#define MAX_TEXT_BUFFER_LENGTH 1024
#endif

    typedef union RectangleU
    {
        struct
        {
            rl::Vector2 pos, size;
        };
        struct
        {
            rl::Rectangle rectangle;
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
        void Fill(rl::Color color) const;
        void Blit(Surface *surface, rl::Vector2 offset = {0, 0}) const;
        void Blit(const rl::Texture2D *texture, rl::Vector2 offset = {0, 0}, RectangleU area = {}) const;
        // Returns the size of the Surface, not the atlas position
        [[nodiscard]] RectangleU GetRect() const;
        rl::Texture2D *Texture();

        static Surface *Load(const char *path);

        RectangleU atlas_rect; // atlas position
        rl::RenderTexture2D render_texture; // atlas texture
    };

    struct TileInfo
    {
        rl::Vector2 position;
        Surface *surface;
    };

    // Warns if there is a render already active
    void BeginTextureModeSafe(const rl::RenderTexture2D &render); // Resets active render
    void EndTextureModeSafe();
    // Starts a render with a Clear color
    void BeginTextureModeC(const rl::RenderTexture2D &render, rl::Color color);
    // Starts drawing with a Clear color
    void BeginDrawingC(rl::Color color);

    // raylib has 4 buffers by default in TextFormat() - to add more, need to recompile raylib.
    // This function receives a buffer created by the application.
    // buffer max size is defined by MAX_TEXT_BUFFER_LENGTH
    void TextFormatSafe(char *buffer, const char *format, ...);

    // Returns center of rectangle
    rl::Vector2 GetRectCenter(RectangleU rect);
    // Returns mid-bottom of rectangle
    rl::Vector2 GetRectMidBottom(RectangleU rect);
    // Returns mid-top of rectangle
    rl::Vector2 GetRectMidTop(RectangleU rect);
    // Returns mid-left of rectangle
    rl::Vector2 GetRectMidLeft(RectangleU rect);
    // Returns mid-right of rectangle
    rl::Vector2 GetRectMidRight(RectangleU rect);
    // Returns top-left of rectangle
    rl::Vector2 GetRectTopLeft(RectangleU rect);
    // Returns top-right of rectangle
    rl::Vector2 GetRectTopRight(RectangleU rect);
    // Returns bottom-left of rectangle
    rl::Vector2 GetRectBottomLeft(RectangleU rect);
    // Returns bottom-right of rectangle
    rl::Vector2 GetRectBottomRight(RectangleU rect);
    // move rectangle's center to position
    void RectToCenter(RectangleU &rect, rl::Vector2 pos);
    // move rectangle's mid-bottom to position
    void RectToMidBottom(RectangleU &rect, rl::Vector2 pos);
    // move rectangle's mid-left to position
    void RectToMidLeft(RectangleU &rect, rl::Vector2 pos);
    // move rectangle's bottom-left to position
    void RectToBottomLeft(RectangleU &rect, rl::Vector2 pos);
    // move rectangle's top-left to position
    void RectToTopLeft(RectangleU &rect, rl::Vector2 pos);
    // move rectangle's top-right to position
    void RectToTopRight(RectangleU &rect, rl::Vector2 pos);
    // Increase/Decrease size of rect, keeping center position
    void RectInflate(RectangleU &rect, float width, float height);

    std::vector<Surface *> ImportFolder(const char *path);

    // get the tile image from the tileset
    Surface *GetTMXTileSurface(const rl::tmx_tile *tile);
    // get a vector with tile info (position on the layer and surface image)
    std::vector<TileInfo> GetTMXTiles(const rl::tmx_map *map, const rl::tmx_layer *layer);
    // merges all tiles into one single surface image
    Surface *GetTMXLayerSurface(const rl::tmx_map *map, const rl::tmx_layer *layer);

    namespace sprite
    {
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
            virtual void LeaveOtherGroups(const SpriteGroup *sprite_group);
            // removes sprite from group and mark for deletion
            virtual void Kill();
            // Flip Horizontally (-width)
            virtual void FlipH();

            unsigned int z = 0; // in 2D games, used to sort the drawing order

            RectangleU rect{}; // world position
            Surface *image = nullptr;
            std::vector<SpriteGroup *> groups{}; // groups that this sprite is in
        };
    } // namespace sprite

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
        Timer(float duration, bool repeat = false, bool autostart = false, const std::function<void()> &func = [] {});
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

    enum Axis
    {
        HORIZONTAL = 0,
        VERTICAL
    };

    void DrawRect(const Surface *surface, rl::Color color, RectangleU rect, float lineThick = 0.0f);
    void DrawCirc(const Surface *surface, rl::Color color, rl::Vector2 center, float radius, float lineThick = 0.0f);

    namespace display
    {
        Surface *SetMode(int width, int height);
        void SetCaption(const char *title);
        Surface *GetSurface();
        void Update();
    } // namespace display

    namespace time
    {
        class Clock
        {
        public:

            Clock();

            // Gets frame time, sets FPS if passed value
            float tick(int fps);
        };
    } // namespace time

} // namespace rg

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
