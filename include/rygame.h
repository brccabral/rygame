// ReSharper disable CppClassCanBeFinal
#pragma once
#include <functional>
#include <list>
#include <utility>
#include <vector>
#include <cstdio>
#include <cstring>
#include <cstdarg>
#include <filesystem>
#include <ctime>
#include <map>
#include <random>

namespace rl
{
#include <raylib.h>
#include <raylib-tmx.h>
} // namespace rl

namespace rg
{
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

    namespace math
    {
        typedef union Vector2
        {
            rl::Vector2 vector2;
            struct
            {
                float x;
                float y;
            };

            [[nodiscard]] float magnitude() const;
            [[nodiscard]] Vector2 normalize() const;
            void normalize_ip();
            float operator[](const unsigned int &i) const;
        } Vector2;

        std::uniform_real_distribution<float> random_uniform_dist(float min, float max);
        float random_uniform(std::uniform_real_distribution<float> dist);
        float clamp(float value, float min, float max);
    } // namespace math

    typedef union Line
    {
        struct
        {
            math::Vector2 start;
            math::Vector2 end;
        };
        struct
        {
            float x1;
            float y1;
            float x2;
            float y2;
        };

        [[nodiscard]] bool collidepoint(math::Vector2 point, float threshold = 0.0f) const;
        [[nodiscard]] bool collideline(Line other, math::Vector2 *collisionPoint) const;
    } Line;

    typedef union Rect
    {
        rl::Rectangle rectangle;

        struct
        {
            math::Vector2 pos, size;
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
        [[nodiscard]] math::Vector2 center() const;
        Rect center(math::Vector2 pos);
        [[nodiscard]] float top() const;
        Rect top(float v);
        [[nodiscard]] float bottom() const;
        Rect bottom(float v);
        [[nodiscard]] math::Vector2 topleft() const;
        Rect topleft(math::Vector2 pos);
        [[nodiscard]] math::Vector2 bottomleft() const;
        Rect bottomleft(math::Vector2 pos);
        [[nodiscard]] math::Vector2 topright() const;
        Rect topright(math::Vector2 pos);
        [[nodiscard]] math::Vector2 bottomright() const;
        Rect bottomright(math::Vector2 pos);
        [[nodiscard]] math::Vector2 midbottom() const;
        Rect midbottom(math::Vector2 pos);
        [[nodiscard]] math::Vector2 midtop() const;
        Rect midtop(math::Vector2 pos);
        [[nodiscard]] math::Vector2 midleft() const;
        Rect midleft(math::Vector2 pos);
        [[nodiscard]] math::Vector2 midright() const;
        Rect midright(math::Vector2 pos);
        [[nodiscard]] Rect inflate(float width, float height) const;
        [[nodiscard]] Rect scale_by(float ratio) const;
        void inflate_ip(float width, float height);
        void scale_by_ip(float ratio);
        [[nodiscard]] Rect copy() const;
        [[nodiscard]] bool collidepoint(math::Vector2 point) const;
        [[nodiscard]] bool collideline(
                Line line, math::Vector2 *collisionPoint1, math::Vector2 *collisionPoint2) const;
    } Rect;

    class Surface
    {
    public:

        // Creates a black Surface* on the heap.
        // Make sure to delete it
        static Surface *Create(int width, int height);
        // Creates a Surface* on the heap using. Reuses texture*, no new texture.
        // Make sure to delete it
        static Surface *Create(rl::Texture2D *texture, Rect atlas = {});

        // Unloads render
        virtual ~Surface();

        // Fill this with a color
        void Fill(rl::Color color);
        // Replace all `color` with BLANK
        void SetColorKey(rl::Color color);
        // Blit incoming Surface* into this.
        void
        Blit(const Surface *incoming, math::Vector2 offset,
             rl::BlendMode blend_mode = rl::BLEND_ALPHA);
        // Blit incoming Texture2D into surface*.
        void
        Blit(const rl::Texture2D &incoming_texture, math::Vector2 offset, Rect area = {},
             rl::BlendMode blend_mode = rl::BLEND_ALPHA);
        // Blit many surfaces into this. `blit_sequence` is a vector of pairs of incoming
        // surface* and offset
        void
        Blits(const std::vector<std::pair<Surface *, math::Vector2>> &blit_sequence,
              rl::BlendMode blend_mode = rl::BLEND_ALPHA);
        // Creates a new Surface*.
        // Make sure to delete it
        [[nodiscard]] Surface *convert(rl::PixelFormat format) const;
        // Returns the atlas size
        [[nodiscard]] Rect GetRect() const;

        // Returns shared_texture if exists, render.texture otherwise.
        [[nodiscard]] rl::Texture2D GetTexture() const;

        // Ends current render, sets this render as current
        void ToggleRender();

        rl::RenderTexture2D render{};
        Rect atlas_rect{}; // atlas position
        // used when a texture comes from a different object
        rl::Texture2D *shared_texture = nullptr;

    protected:

        // we must create Surface on heap (use Surface::Create)

        Surface(int width, int height);
        Surface(rl::Texture2D *texture, Rect atlas);
        Surface(const Surface &other) = delete;
        Surface(Surface &&other) = delete;
        Surface &operator=(const Surface &other) = delete;
        Surface &operator=(Surface &&other) = delete;
    };

    class Frames : public Surface
    {
    public:

        static Frames *Create(int width, int height, int rows, int cols);
        static Frames *Create(const Surface *surface, int rows, int cols);

        // Set current atlas rect. Default to first frame.
        // Value is moduled with frame length in case it is greater than frames size.
        void SetAtlas(unsigned int frame_index = 0);
        // Merge a list of Surfaces. Assumes all surfaces are same size.
        // Caller must delete returned Frame*
        static Frames *Merge(const std::vector<Surface *> &surfaces, int rows, int cols);

        unsigned int current_frame_index{};
        std::vector<Rect> frames{};

    protected:

        // Width/Height is the total size of the image
        // Rows/Cols will create atlas vector with N=rows*cols, each N of
        // size (Width/Cols, Height/Rows)
        Frames(int width, int height, int rows, int cols);
    };

    namespace image
    {
        // Load a file into a new Surface*
        // Make sure to delete it
        Surface *Load(const char *path);
        // Loads all files in a folder and returns a vector<> of new Surface*
        // Make sure to delete them
        std::vector<Surface *> LoadFolderList(const char *path);
        // Loads all files in a folder and returns a map<> (dictionary) of new Surface*
        // where the key is the filename
        // Make sure to delete them
        std::map<std::string, Surface *> LoadFolderDict(const char *path);
        // Walk a folder path and loads all images
        // Returns a vector of Surface*
        // The caller must delete Surface*
        std::vector<Surface *> ImportFolder(const char *path);
        // Walk a folder path and loads all images
        // Returns a map where the key is filename and values are Surface*
        // The caller must delete Surface*
        std::map<std::string, Surface *> ImportFolderDict(const char *path);
        // Delete all surfaces in vector
        void DeleteAllVector(const std::vector<Surface *> &surfaces);
        // Delete all surfaces in map
        template<typename K>
        void DeleteAllMap(const std::map<K, Surface *> &surfaces)
        {
            for (auto &[key, surf]: surfaces)
            {
                delete surf;
            }
        };
    } // namespace image

    namespace draw
    {
        void
        rect(Surface &surface, rl::Color color, Rect rect, float lineThick = 0.0f,
             float radius = 0.0f, bool topLeft = true, bool topRight = true, bool bottomLeft = true,
             bool bottomRight = true);
        void
        circle(Surface &surface, rl::Color color, math::Vector2 center, float radius,
               float lineThick = 0.0f);
        void
        bar(Surface &surface, Rect rect, float value, float max_value, rl::Color color,
            rl::Color bg_color, float radius = 0.0f);
    } // namespace draw

    namespace tmx
    {
        // World Position, Atlas image*, Atlas position
        struct TileInfo
        {
            math::Vector2 position{}; // position on screen (x*tileSize, y*tileSize)
            rl::Texture2D *texture = nullptr; // if tile has image, allocate it in memory
            Rect atlas_rect{};
        };
        // get the tile image from the tileset
        rl::Texture2D *GetTMXTileTexture(const rl::tmx_tile *tile, Rect *atlas_rect);
        // get a vector with tile info (position on the layer and surface image)
        std::vector<TileInfo> GetTMXTiles(const rl::tmx_map *map, const rl::tmx_layer *layer);
        // merges all tiles into one single surface image
        Surface *GetTMXLayerSurface(const rl::tmx_map *map, const rl::tmx_layer *layer);
    } // namespace tmx

    namespace sprite
    {
        class Sprite; // forward declaration

        // Manages multiple sprites at once
        class Group
        {
        public:

            // Group cannot be allocated in Heap
            void *operator new(size_t) = delete;

            virtual ~Group() = default;

            // Draw all sprites into surface
            virtual void Draw(Surface &surface);
            // Updates all sprites
            void Update(float deltaTime) const;
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
            [[nodiscard]] std::vector<Sprite *> Sprites() const;


        protected:

            std::vector<Sprite *> sprites{};
        };

        class SpriteOwner
        {
        public:

            SpriteOwner() = default;
            ~SpriteOwner();

            void add(Sprite *sprite);
            void remove(Sprite *sprite);
            bool has(const Sprite *check_sprite);

        private:

            std::vector<Sprite *> sprites;
        };

        class Sprite
        {
        public:

            Sprite(Group *to_add_group, SpriteOwner *owner);
            Sprite(const std::vector<Group *> &groups, SpriteOwner *owner);
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
            // removes sprite from group. Returns current Sprite*
            // If discarded, will call ~Sprite(). Capture it to not call ~Sprite()
            [[nodiscard]] virtual Sprite *Kill();
            // Flip Horizontally (-width)
            void FlipH() const;

            void ReplaceOwner(SpriteOwner *replace);

            unsigned int z = 0; // in 2D games, used to sort the drawing order

            Rect rect{}; // world position
            Surface *image = nullptr;

            SpriteOwner *owner = nullptr;

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
                Sprite *sprite, const Group *group, bool dokill,
                const std::function<bool(Sprite *left, Sprite *right)> &collided = collide_rect);

        // Tests if Sprite collides with any sprite in group, returns the first sprite in
        // group that collides
        Sprite *spritecollideany(
                Sprite *sprite, const Group *group,
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

        bool repeat{};
        bool autostart{};
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
            static float tick(int fps = 0.0f);
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
            Rect atlas_rect{};
        };

        Mask FromSurface(const Surface *surface, unsigned char threshold = 127);
    } // namespace mask

    namespace font
    {
        class Font
        {
        public:

            // Default Font, size 1
            Font();
            // Load font from file
            Font(const char *file, float font_size);
            // Raylib Font
            Font(rl::Font font, float font_size);
            ~Font();
            // Creates a Text surface from this Font. Make sure to delete it.
            // If passed padding_width or padding_height, surface dimensions will be added
            // (textsize + (width,height))
            Surface *
            render(const char *text, rl::Color color, float spacing = 1, rl::Color bg = rl::BLANK,
                   float padding_width = 0, float padding_height = 0) const;
            math::Vector2 size(const char *text) const;

        private:

            rl::Font font;
            float font_size;
        };
    } // namespace font

    namespace mixer
    {
        class Sound
        {
        public:

            Sound() = default;
            explicit Sound(const char *file, bool isMusic = false);
            ~Sound();

            void Play() const;
            void Stop() const;
            void SetVolume(float value) const;
            [[nodiscard]] const char *GetFilename() const;

            void *audio = nullptr;

        private:

            bool isMusic{};
            const char *file = nullptr;
        };
    } // namespace mixer

    static std::vector<mixer::Sound *> musics;
} // namespace rg

rg::math::Vector2 operator+(const rg::math::Vector2 &lhs, const rg::math::Vector2 &rhs);
rg::math::Vector2 operator-(const rg::math::Vector2 &lhs, const rg::math::Vector2 &rhs);
rg::math::Vector2 &operator+=(rg::math::Vector2 &lhs, const rg::math::Vector2 &rhs);
rg::math::Vector2 &operator-=(rg::math::Vector2 &lhs, const rg::math::Vector2 &rhs);
rg::math::Vector2 operator*(const rg::math::Vector2 &lhs, float scale);
rg::math::Vector2 &operator*=(rg::math::Vector2 &lhs, float scale);
