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
#include <map>

namespace rl
{
#include <raylib.h>
#include <raymath.h>
#include <raylib-tmx.h>
} // namespace rl

rl::Vector2 operator+(const rl::Vector2 &lhs, const rl::Vector2 &rhs);
rl::Vector2 &operator+=(rl::Vector2 &lhs, const rl::Vector2 &rhs);
rl::Vector2 &operator-=(rl::Vector2 &lhs, const rl::Vector2 &rhs);
rl::Vector2 operator*(const rl::Vector2 &lhs, float scale);

namespace rg
{
    void
    Init(int logLevel = rl::LOG_WARNING, unsigned int config_flags = 0,
         rl::TraceLogCallback callback = nullptr);
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
        void
        Blit(const rl::Texture2D *texture, rl::Vector2 offset = {0, 0}, RectangleU area = {}) const;
        // Returns the size of the Surface, not the atlas position
        [[nodiscard]] RectangleU GetRect() const;
        rl::Texture2D *Texture();
        void SetColorKey(rl::Color color) const;

        // Load a file into a Surface*
        // The caller must delete Surface*
        static Surface *Load(const char *path);

        RectangleU atlas_rect{}; // atlas position
        rl::RenderTexture2D render_texture{}; // atlas texture
    };

    struct TileInfo
    {
        rl::Vector2 position{}; // position on screen (x*tileSize, y*tileSize)
        Surface *surface = nullptr; // if tile has image, allocate it in memory
    };

    // Warns if there is a render already active
    void BeginTextureModeSafe(const rl::RenderTexture2D &render); // Resets active render
    void EndTextureModeSafe();
    // Starts a render with a Clear color
    void BeginTextureModeC(const rl::RenderTexture2D &render, rl::Color color);
    // Starts drawing with a Clear color
    void BeginDrawingC(rl::Color color);

    // raylib has 4 buffers by default in TextFormat() - to add more, need to recompile
    // raylib. This function receives a buffer created by the application. buffer max size
    // is defined by MAX_TEXT_BUFFER_LENGTH
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
    // Increase/Decrease size of rect, keeping center position
    void RectInflate(RectangleU &rect, float ratio);

    namespace assets
    {
        // Walk a folder path and loads all images
        // Returns a vector of Surface*
        // The caller must delete Surface*
        std::vector<Surface *> ImportFolder(const char *path);
        // Walk a folder path and loads all images
        // Returns a map where the key is filename and values are Surface*
        // The caller must delete Surface*
        std::map<std::string, Surface *> ImportFolderDict(const char *path);
    } // namespace assets

    namespace tmx
    {
        // get the tile image from the tileset
        Surface *GetTMXTileSurface(const rl::tmx_tile *tile);
        // get a vector with tile info (position on the layer and surface image)
        std::vector<TileInfo> GetTMXTiles(const rl::tmx_map *map, const rl::tmx_layer *layer);
        // merges all tiles into one single surface image
        Surface *GetTMXLayerSurface(const rl::tmx_map *map, const rl::tmx_layer *layer);
    } // namespace tmx

    // Generate image with random pixel colors
    rl::Image GenImageRandomPixels(float width, float height);

    namespace sprite
    {
        class Sprite;

        // Manages multiple sprites at once
        class Group
        {
        public:

            virtual ~Group();
            // Draw all sprites into surface
            virtual void Draw(Surface *surface);
            // Updates all sprites
            void Update(float deltaTime);
            // Removes all sprites from Group
            void empty();
            // Removes a list of sprites from this group (if they are part of this group)
            void remove(const std::vector<Sprite *> &to_remove_sprites);
            // Removes a Sprite from this group if it is in this group
            void remove(Sprite *to_remove_sprite);
            // Adds a list of sprites to this group
            void add(const std::vector<Sprite *> &to_add_sprites);
            // Adds a Sprite to this group
            void add(Sprite *to_add_sprite);

            std::vector<Sprite *> sprites{};
            std::vector<Sprite *> to_delete{};

        private:

            void DeleteAll();
        };

        class Sprite
        {
        public:

            // Pass group by reference because the sprite does not own the group
            explicit Sprite(Group *group);
            // Pass group by reference because the sprite does not own the group
            explicit Sprite(const std::vector<Group *> &groups);
            virtual ~Sprite();

            // add this sprite to passed group
            void add(Group *to_add_group);
            // add this sprite to all groups
            void add(const std::vector<Group *> &to_add_groups);
            // remove group from this sprite
            void remove(Group *to_remove_group);
            // remove all groups from this sprite
            void remove(const std::vector<Group *> &to_remove_groups);

            virtual void Update(float deltaTime){};
            virtual void LeaveOtherGroups(const Group *not_leave_group);
            // removes sprite from group and mark for deletion
            virtual void Kill();
            // Flip Horizontally (-width)
            virtual void FlipH();

            unsigned int z = 0; // in 2D games, used to sort the drawing order

            RectangleU rect{}; // world position
            Surface *image = nullptr;
            std::vector<Group *> groups{}; // groups that this sprite is in
        };

        bool collide_rect(const Sprite *left, const Sprite *right);

        class CollideCallable
        {
        public:

            CollideCallable() = default;

            virtual bool operator()(const Sprite *left, const Sprite *right) const = 0;

        protected:

            virtual ~CollideCallable() = default;
        };

        class collide_rect_ratio : public CollideCallable
        {
        public:

            explicit collide_rect_ratio(float ratio);
            bool operator()(const Sprite *left, const Sprite *right) const override;

        private:

            float ratio;
        };


        // Returns a list of all sprites in the group that collides with the sprite
        // If dokill is true, all collided sprites are removed from group
        std::vector<Sprite *> spritecollide(
                Sprite *sprite, Group *group, bool dokill,
                const std::function<bool(Sprite *left, Sprite *right)> &collided = collide_rect);

        // Tests if Sprite collides with any sprite in group, returns the first sprite in
        // group that collides
        Sprite *spritecollideany(
                Sprite *sprite, Group *group,
                const std::function<bool(Sprite *left, Sprite *right)> &collided = collide_rect);
    } // namespace sprite

    // Remains active for certain duration, can repeat once it is done, can autostart
    // and calls a func at the end. Remember to call Update() on every frame
    class Timer
    {
    public:

        // Default constructor
        Timer() : duration(0.0f), repeat(false), autostart(false), func(nullptr)
        {}

        // Parameterized constructor
        explicit
        Timer(float duration, bool repeat = false, bool autostart = false,
              const std::function<void()> &func = nullptr);
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
    void DrawCirc(
            const Surface *surface, rl::Color color, rl::Vector2 center, float radius,
            float lineThick = 0.0f);

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

            Clock() = default;

            // Gets frame time, sets FPS if passed value
            static float tick(int fps);
        };
    } // namespace time

    namespace mask
    {
        class Mask
        {
        public:

            Mask(unsigned int width, unsigned int height, bool fill = false);
            ~Mask();
            [[nodiscard]] Surface *ToSurface() const;

            rl::Image image{};
        };

        Mask FromSurface(Surface *surface, unsigned char threshold = 127);
    } // namespace mask
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
