// ReSharper disable CppClassCanBeFinal
#pragma once
#include <functional>
#include <list>
#include <utility>
#include <vector>
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <cstdarg>
#include <filesystem>
#include <ctime>
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
    // using std::function;
    // using std::initializer_list;
    // using std::list;
    // using std::map;
    // using std::pair;
    // using std::string;
    // using std::unordered_map;
    // using std::va_list;
    // using std::vector;

    enum Axis
    {
        HORIZONTAL = 0,
        VERTICAL
    };

    void
    Init(int logLevel = rl::LOG_WARNING, unsigned int config_flags = 0,
         rl::TraceLogCallback callback = nullptr);
    void Quit();

    // Warns if there is a render already active
    void BeginTextureModeSafe(const rl::RenderTexture2D &render); // Resets active render
    void EndTextureModeSafe();
    // Starts a render with a Clear color
    void BeginTextureModeC(const rl::RenderTexture2D &render, rl::Color color);
    // Starts drawing with a Clear color
    void BeginDrawingC(rl::Color color);

    // Generate image with random pixel colors
    rl::Image GenImageRandomPixels(float width, float height);

#ifndef MAX_TEXT_BUFFER_LENGTH
#define MAX_TEXT_BUFFER_LENGTH 1024
#endif

    // raylib has 4 buffers by default in TextFormat() - to add more, need to recompile
    // raylib. This function receives a buffer created by the application. buffer max size
    // is defined by MAX_TEXT_BUFFER_LENGTH
    void TextFormatSafe(char *buffer, const char *format, ...);

    template<typename K, typename C>
    std::vector<K> getKeys(C &map)
    {
        std::vector<K> keys;
        keys.reserve(map.size());

        for (const auto &pair: map)
        {
            keys.push_back(pair.first);
        }

        return keys;
    }
    template<typename K, typename C>
    std::vector<K> getValues(C &map)
    {
        std::vector<K> values;
        values.reserve(map.size());

        for (const auto &pair: map)
        {
            values.push_back(pair.second);
        }

        return values;
    }

    typedef union Rect
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

        [[nodiscard]] float right() const;
        Rect right(float v);
        [[nodiscard]] float left() const;
        Rect left(float v);
        [[nodiscard]] float centerx() const;
        Rect centerx(float v);
        [[nodiscard]] float centery() const;
        Rect centery(float v);
        [[nodiscard]] rl::Vector2 center() const;
        Rect center(rl::Vector2 pos);
        [[nodiscard]] float top() const;
        Rect top(float v);
        [[nodiscard]] float bottom() const;
        Rect bottom(float v);
        [[nodiscard]] rl::Vector2 topleft() const;
        Rect topleft(rl::Vector2 pos);
        [[nodiscard]] rl::Vector2 bottomleft() const;
        Rect bottomleft(rl::Vector2 pos);
        [[nodiscard]] rl::Vector2 topright() const;
        Rect topright(rl::Vector2 pos);
        [[nodiscard]] rl::Vector2 bottomright() const;
        Rect bottomright(rl::Vector2 pos);
        [[nodiscard]] rl::Vector2 midbottom() const;
        Rect midbottom(rl::Vector2 pos);
        [[nodiscard]] rl::Vector2 midtop() const;
        Rect midtop(rl::Vector2 pos);
        [[nodiscard]] rl::Vector2 midleft() const;
        Rect midleft(rl::Vector2 pos);
        [[nodiscard]] rl::Vector2 midright() const;
        Rect midright(rl::Vector2 pos);
        [[nodiscard]] Rect inflate(float width, float height) const;
        [[nodiscard]] Rect scale_by(float ratio) const;
        void inflate_ip(float width, float height);
        void scale_by_ip(float ratio);
        [[nodiscard]] Rect copy() const;
    } Rect;

    // Map like container, but keeps order as it was inserted, not based on `keys` as `std::map`
    template<typename K, typename V>
    class InsertOrderMap
    {
    public:

        InsertOrderMap() = default;
        InsertOrderMap(std::initializer_list<std::pair<K, V>> init);

        [[nodiscard]] unsigned int size() const;
        void insert(const K &key, const V &value);
        V &operator[](const K &key);
        typename std::list<std::pair<K, V>>::iterator begin();
        typename std::list<std::pair<K, V>>::iterator end();

    private:

        std::list<std::pair<K, V>> order_;
        // the `map_` points to an element inside `order_`, not to the value. The value is stored in
        // `order_` only
        std::unordered_map<K, typename std::list<std::pair<K, V>>::iterator> map_;
    };

    // !!!! template<> classes must have definitions in .h files
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
    unsigned int InsertOrderMap<K, V>::size() const
    {
        return map_.size();
    }

    template<typename K, typename V>
    void InsertOrderMap<K, V>::insert(const K &key, const V &value)
    {
        auto it = map_.find(key);
        if (it != map_.end())
        {
            it->second->second = value; // Update value inside `order_`
        }
        else
        {
            order_.emplace_back(key, value);
            map_[key] = --order_.end(); // Point to the `order_` location
        }
    }

    template<typename K, typename V>
    V &InsertOrderMap<K, V>::operator[](const K &key)
    {
        auto it = map_.find(key);
        if (it != map_.end())
        {
            return it->second->second; // returns value inside `order_`
        }
        order_.emplace_back(key, V{}); // create a new default value
        map_[key] = --order_.end(); // Point to the `order_` location
        return map_[key]->second; // return the value
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

    class Surface
    {
    public:

        Surface(int width, int height);
        ~Surface();
        void Fill(rl::Color color) const;
        void
        Blit(Surface *surface, rl::Vector2 offset = {0, 0},
             rl::BlendMode blend_mode = rl::BLEND_ALPHA) const;
        void
        Blit(const rl::Texture2D *texture, rl::Vector2 offset = {0, 0}, Rect area = {},
             rl::BlendMode blend_mode = rl::BLEND_ALPHA) const;
        // Returns the size of the Surface, not the atlas position
        [[nodiscard]] Rect GetRect() const;
        rl::Texture2D *Texture();
        void SetColorKey(rl::Color color) const;

        // Load a file into a Surface*
        // The caller must delete Surface*
        static Surface *Load(const char *path);

        Rect atlas_rect{}; // atlas position
        rl::RenderTexture2D render_texture{}; // atlas texture
    };

    namespace draw
    {
        void
        rect(const Surface *surface, rl::Color color, Rect rect, float lineThick = 0.0f,
             float radius = 0.0f);
        void
        circle(const Surface *surface, rl::Color color, rl::Vector2 center, float radius,
               float lineThick = 0.0f);
    } // namespace draw

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
        struct TileInfo
        {
            rl::Vector2 position{}; // position on screen (x*tileSize, y*tileSize)
            Surface *surface = nullptr; // if tile has image, allocate it in memory
        };
        // get the tile image from the tileset
        Surface *GetTMXTileSurface(const rl::tmx_tile *tile);
        // get a vector with tile info (position on the layer and surface image)
        std::vector<TileInfo> GetTMXTiles(const rl::tmx_map *map, const rl::tmx_layer *layer);
        // merges all tiles into one single surface image
        Surface *GetTMXLayerSurface(const rl::tmx_map *map, const rl::tmx_layer *layer);
    } // namespace tmx

    namespace sprite
    {
        class Sprite;

        // Manages multiple sprites at once
        class Group
        {
        public:

            // Group cannot be allocated in Heap
            void *operator new(size_t) = delete;

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
            // Check if all sprites are in group
            bool has(const std::vector<Sprite *> &check_sprites);
            // Check if sprite is in group
            bool has(const Sprite *check_sprite);
            // Returns a copy of vector sprites
            std::vector<Sprite *> Sprites();

        protected:

            std::vector<Sprite *> sprites{};
        };

        class Sprite
        {
        public:

            // Pass group by reference because the sprite does not own the group
            explicit Sprite(Group *to_add_group);
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
            // Returns a copy of the list of groups
            std::vector<Group *> Groups();

            virtual void Update(float deltaTime){};
            // removes sprite from group. If passed false, you must
            // capture the returned value and delete it later. If passed true,
            // it will mark for deletion and return `nullptr`
            virtual Sprite *Kill(bool deleteSprite);
            // Flip Horizontally (-width)
            virtual void FlipH();

            unsigned int z = 0; // in 2D games, used to sort the drawing order

            Rect rect{}; // world position
            Surface *image = nullptr;

        protected:

            std::vector<Group *> groups{}; // groups that this sprite is in
        private:

            bool has(const Group *check_group);
            // Leave groups that are not the passed one
            virtual void LeaveOtherGroups(const Group *not_leave_group);
            // leave all groups
            void LeaveAllGroups();
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

    namespace font
    {
        class Font
        {
        public:

            // Default Font, size 1
            Font();
            // Load font from file
            Font(const char *file, float size);
            // Raylib Font
            Font(rl::Font font, float size);
            ~Font();
            // Creates a Text surface from this Font. Make sure to delete it.
            Surface *render(const char *text, rl::Color color, float spacing = 1) const;

        private:

            rl::Font font;
            float size;
        };
    } // namespace font
} // namespace rg
