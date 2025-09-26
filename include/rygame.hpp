// ReSharper disable CppClassCanBeFinal
#pragma once
#include <algorithm>
#include <cstdarg>
#include <cstdio>
#include <cstring>
#include <ctime>
#include <filesystem>
#include <functional>
#include <list>
#include <map>
#include <random>
#include <ranges>
#include <sstream>
#include <utility>
#include <vector>

namespace rl
{
#ifdef _WIN32
#define NOSOUND
#define MMNOSOUND
#define MA_NO_WINMM
#endif
#include <raylib.h>
#include <raymath.h>
#ifdef WITH_TMX
#include <raylib-tmx.h>
#endif // WITH_TMX
} // namespace rl

namespace rg
{
    enum Axis
    {
        HORIZONTAL = 0,
        VERTICAL
    };

    void
    Init(
            int logLevel = rl::LOG_WARNING, unsigned int config_flags = 0,
            rl::TraceLogCallback callback = nullptr);
    void Quit();
    bool WindowCloseOrQuit();

    // Warns if there is a render already active
    void BeginTextureModeSafe(const rl::RenderTexture2D &render); // Resets active render
    void EndTextureModeSafe();
    rl::Texture2D LoadTextureSafe(const char *file);
    rl::Texture2D LoadTextureFromImageSafe(const rl::Image &image);
    rl::Image LoadImageFromTextureSafe(const rl::Texture &texture);
    void UnloadTextureSafe(const rl::Texture2D &texture);
    rl::RenderTexture2D LoadRenderTextureSafe(int width, int height);
    void UnloadRenderTextureSafe(const rl::RenderTexture2D &render);
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

    // Split string in a vector<string>
    std::vector<std::string> Split(const std::string &s, char delim);

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

    template<typename K, typename V, typename C>
    std::vector<std::pair<K, V>> getItems(C &map)
    {
        std::vector<std::pair<K, V>> items;
        items.reserve(map.size());

        for (const auto &pair: map)
        {
            items.push_back(pair);
        }

        return items;
    }

    // Map like container, but keeps order as it was inserted, not based on `keys` as `std::map`
    template<typename K, typename V>
    class InsertOrderMap
    {
    public:

        InsertOrderMap() = default;
        InsertOrderMap(const std::initializer_list<std::pair<K, V>> &init);

        [[nodiscard]] unsigned int size() const;
        void insert(const K &key, const V &value);
        V &operator[](const K &key);
        typename std::list<std::pair<K, V>>::const_iterator cbegin() const;
        typename std::list<std::pair<K, V>>::const_iterator cend() const;
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
    InsertOrderMap<K, V>::InsertOrderMap(const std::initializer_list<std::pair<K, V>> &init)
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
    typename std::list<std::pair<K, V>>::const_iterator InsertOrderMap<K, V>::cbegin() const
    {
        return order_.cbegin();
    }

    template<typename K, typename V>
    typename std::list<std::pair<K, V>>::const_iterator InsertOrderMap<K, V>::cend() const
    {
        return order_.cend();
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
        // GCC warns about Anonymous Struct
#if !_WIN32
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#endif
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
            [[nodiscard]] float distance_to(Vector2 target) const;
            float operator[](const unsigned int &i) const;
            explicit operator bool() const;
        } Vector2;

        typedef union Vector3i
        {
            int x, y, z;
        } Vector3i;

        typedef union Vector3uc
        {
            struct
            {
                unsigned char x{}, y{}, z{};
            };

            struct
            {
                unsigned char r, g, b;
            };

            explicit Vector3uc(rl::Vector3 v);
        } Vector3uc;
#if !_WIN32
#pragma GCC diagnostic pop
#endif

        std::mt19937 *get_rng();
        std::uniform_real_distribution<float> random_uniform_dist(float min, float max);
        float random_uniform(std::uniform_real_distribution<float> dist);
        float get_random_uniform(float min, float max);
        float clamp(float value, float min, float max);
        std::uniform_int_distribution<int> random_uniform_dist(int min, int max);
        int random_uniform(std::uniform_int_distribution<int> dist);
        int get_random_uniform(int min, int max);
        int clamp(int value, int min, int max);
    } // namespace math

    // GCC warns about Anonymous Struct
#if !_WIN32
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#endif
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
        explicit operator bool() const;
    } Line;
#if !_WIN32
#pragma GCC diagnostic pop
#endif

    // GCC warns about Anonymous Struct
#if !_WIN32
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#endif
    typedef union Rect
    {
        struct
        {
            math::Vector2 pos, size;
        };

        rl::Rectangle rectangle;

        struct
        {
            float x{}, y{}, width{}, height{};
        };

        Rect() = default;
        Rect(math::Vector2 pos, math::Vector2 size);
        explicit Rect(rl::Rectangle rect);
        Rect(float x, float y, float width, float height);

        // Returns the X value on the right side (x+width)
        [[nodiscard]] float right() const;
        // Moves the rect so that the right side goes to the passed position
        // and returns a copy
        Rect right(float v);
        // Returns the X value on the leftside (x)
        [[nodiscard]] float left() const;
        // Moves the rect so that the left side goes to the passed position
        // and returns a copy
        Rect left(float v);
        // Returns the X value of the center
        [[nodiscard]] float centerx() const;
        // Moves the rect so that the center X goes to the passed position
        // and returns a copy
        Rect centerx(float v);
        // Returns the Y value of the center
        [[nodiscard]] float centery() const;
        // Moves the rect so that the center Y goes to the passed position
        // and returns a copy
        Rect centery(float v);
        // Returns the x,y at the center
        [[nodiscard]] math::Vector2 center() const;
        // Moves the rect so that the center goes to the passed position
        // and returns a copy
        Rect center(math::Vector2 pos);
        // Returns the Y value of the top
        [[nodiscard]] float top() const;
        // Moves the rect so that the top Y goes to the passed position
        // and returns a copy
        Rect top(float v);
        // Returns the Y value of the bottom
        [[nodiscard]] float bottom() const;
        // Moves the rect so that the bottom Y goes to the passed position
        // and returns a copy
        Rect bottom(float v);
        // Returns the x,y at the top left
        [[nodiscard]] math::Vector2 topleft() const;
        // Moves the rect so that the top left goes to the passed position
        // and returns a copy
        Rect topleft(math::Vector2 pos);
        // Returns the x,y at the bottom left
        [[nodiscard]] math::Vector2 bottomleft() const;
        // Moves the rect so that the bottom left goes to the passed position
        // and returns a copy
        Rect bottomleft(math::Vector2 pos);
        // Returns the x,y at the top right
        [[nodiscard]] math::Vector2 topright() const;
        // Moves the rect so that the top right goes to the passed position
        // and returns a copy
        Rect topright(math::Vector2 pos);
        // Returns the x,y at the bottom right
        [[nodiscard]] math::Vector2 bottomright() const;
        // Moves the rect so that the bottom right goes to the passed position
        // and returns a copy
        Rect bottomright(math::Vector2 pos);
        // Returns the x,y at the midbottom
        [[nodiscard]] math::Vector2 midbottom() const;
        // Moves the rect so that the mid bottom goes to the passed position
        // and returns a copy
        Rect midbottom(math::Vector2 pos);
        // Returns the x,y at the midtop
        [[nodiscard]] math::Vector2 midtop() const;
        // Moves the rect so that the mid top goes to the passed position
        // and returns a copy
        Rect midtop(math::Vector2 pos);
        // Returns the x,y at the mid left
        [[nodiscard]] math::Vector2 midleft() const;
        // Moves the rect so that the mid left goes to the passed position
        // and returns a copy
        Rect midleft(math::Vector2 pos);
        // Returns the x,y at the mid right
        [[nodiscard]] math::Vector2 midright() const;
        // Moves the rect so that the mid right goes to the passed position
        // and returns a copy
        Rect midright(math::Vector2 pos);
        // Moves the rect by delta pixels
        // and returns a copy
        Rect move(math::Vector2 delta);
        // Returns a modified rect with increased/decreased sizes, but same center
        // This rect is not modified. Use `inflate_ip` for in-place
        [[nodiscard]] Rect inflate(float width, float height) const;
        // Returns a modified rect with increased/decreased sizes, but same center
        // This rect is not modified. Use `scale_by_ip` for in-place
        [[nodiscard]] Rect scale_by(float ratio) const;
        // Modifies this rect with increased/decreased sizes, keeping the center position
        // This is an in-place change. Use `inflate` to keep original size
        void inflate_ip(float width, float height);
        // Modifies this rect with increased/decreased sizes, keeping the center position
        // This is an in-place change. Use `scale_by` to keep original size
        void scale_by_ip(float ratio);
        // Returns a copy
        [[nodiscard]] Rect copy() const;
        // Returns true if point is inside rect
        [[nodiscard]] bool collidepoint(math::Vector2 point) const;
        // Returns true if line crosses rect (or is entirely inside it)
        // It also updates collision points on the intersections
        [[nodiscard]] bool collideline(
                Line line, math::Vector2 *collisionPoint1, math::Vector2 *collisionPoint2) const;
        // Returns true if other rect overlaps this one
        [[nodiscard]] bool colliderect(const Rect &other) const;
        // If passed line crosses the rect, returns a new line that is just inside the rect
        // If passed line is outside, returns an empty line {}
        Line clipline(Line line) const;
        // If passed line (from start to end) crosses the rect, returns a new line that is just
        // inside the rect. If passed line is outside, returns an empty line {}
        Line clipline(math::Vector2 start, math::Vector2 end) const;
        // If passed line (from x1,y1 to x2,y2) crosses the rect, returns a new line that is just
        // inside the rect. If passed line is outside, returns an empty line {}
        Line clipline(float x1, float y1, float x2, float y2) const;
    } Rect;
#if !_WIN32
#pragma GCC diagnostic pop
#endif

    class Surface
    {
    public:

        Surface() = default;
        Surface(int width, int height);
        explicit Surface(math::Vector2 size);
        explicit Surface(rl::Texture2D *texture, Rect atlas = {});

        Surface(const Surface &other) = delete;
        Surface &operator=(const Surface &other) = delete;
        Surface(Surface &&other) noexcept;
        Surface &operator=(Surface &&other) noexcept;

        // Unloads render
        virtual ~Surface();

        // Fill this with a color
        void Fill(rl::Color color);
        // Replace all `color` with BLANK
        virtual void SetColorKey(rl::Color color);
        // Set Alpha (0.0f to 255.0f) - Changes tint color
        void SetAlpha(float alpha);
        // Blit incoming Surface* into this.
        void
        Blit(
                const Surface *incoming, const Rect &offset,
                rl::BlendMode blend_mode = rl::BLEND_ALPHA, float scale = 1.0f);
        // Blit incoming Surface* into this.
        void
        Blit(
                const Surface *incoming, const math::Vector2 &offset,
                rl::BlendMode blend_mode = rl::BLEND_ALPHA, float scale = 1.0f);
        // Blit incoming Texture2D into surface*.
        void
        Blit(
                const rl::Texture2D &incoming_texture, math::Vector2 offset, Rect area = {},
                rl::BlendMode blend_mode = rl::BLEND_ALPHA, rl::Color tint = rl::WHITE,
                float scale = 1.0f);
        // Blit many surfaces into this. `blit_sequence` is a vector of pairs of incoming
        // surface* and offset
        void
        Blits(
                const std::vector<std::pair<Surface *, math::Vector2>> &blit_sequence,
                rl::BlendMode blend_mode = rl::BLEND_ALPHA);
        // Creates a new Surface*.
        // Make sure to delete it
        [[nodiscard]] Surface convert(rl::PixelFormat format) const;
        [[nodiscard]] Surface copy() const;
        // Returns the atlas size
        [[nodiscard]] Rect GetRect() const;

        // Returns a different Surface, but it shares same image
        // as this one. SubSurface will have this as parent (GetParent, GetAbsParent).
        virtual Surface SubSurface(Rect rect);
        [[nodiscard]] Surface *GetParent() const;
        Surface *GetAbsParent();

        // Returns shared_texture if exists, render.texture otherwise.
        [[nodiscard]] rl::Texture2D GetTexture() const;

        // Ends current render, sets this render as current
        void ToggleRender();

        rl::RenderTexture2D render{};
        Rect atlas_rect{}; // atlas position
        // used when a texture comes from a different object
        rl::Texture2D *shared_texture = nullptr;

    protected:

        void Setup(int width, int height);

        Surface *parent = nullptr;
        math::Vector2 m_offset{};
        float flip_atlas_height = 1; // 1 or -1 (Frames)

        rl::Color m_tint{255, 255, 255, 255};
    };

    class Frames : public Surface
    {
    public:

        Frames() = default;
        // Width/Height is the total size of the image
        // Rows/Cols will create atlas vector with N=rows*cols, each N of
        // size (Width/Cols, Height/Rows)
        Frames(int width, int height, int rows, int cols);
        Frames(const Surface *surface, int rows, int cols);

        // Set current atlas rect. Default to first frame.
        // Value is moduled with frame length in case it is greater than frames size.
        void SetAtlas(int frame_index = 0);
        // Merge a list of Surfaces. Assumes all surfaces are same size.
        // Caller must delete returned Frame*
        static Frames Merge(const std::vector<Surface> &surfaces, int rows, int cols);
        // Load an image and create frames for it
        static Frames Load(const char *file, int rows, int cols);
        void SetColorKey(rl::Color color) override;

        Surface SubSurface(Rect rect) override
        {
            throw std::logic_error("Frame class does not have SubSurface, call SubFrames instead");
        };
        // Returns a different shared_ptr<Frames>, but it shares same image
        // as this one. SubSurface will have this as parent (GetParent, GetAbsParent).
        // SubSurface will have its frames updated.
        Frames SubFrames(Rect rect);

        int current_frame_index{};
        std::vector<Rect> frames{};
        int m_rows{};
        int m_cols{};

    private:

        void CreateFrames(int width, int height, int rows, int cols);
    };

    namespace image
    {
        // Load a file into a new Surface*
        // Make sure to delete it
        Surface Load(const char *path);
        // Loads all files in a folder and returns a vector<> of new Surface*
        // Make sure to delete them
        std::vector<Surface> LoadFolderList(const char *path);
        // Loads all files in a folder and returns a map<> (dictionary) of new Surface*
        // where the key is the filename
        // Make sure to delete them
        std::unordered_map<std::string, Surface> LoadFolderDict(const char *path);
        // Walk a folder path and loads all images
        // Returns a vector of Surface*
        // The caller must delete Surface*
        std::vector<Surface> ImportFolder(const char *path);
        // Walk a folder path and loads all images
        // Returns a map where the key is filename and values are Surface*
        // The caller must delete Surface*
        std::unordered_map<std::string, Surface> ImportFolderDict(const char *path);
    } // namespace image

    namespace draw
    {
        void
        rect(
                Surface *surface, rl::Color color, Rect rect, float lineThick = 0.0f,
                float radius = 0.0f, bool topLeft = true, bool topRight = true,
                bool bottomLeft = true,
                bool bottomRight = true);
        void
        circle(
                Surface *surface, rl::Color color, math::Vector2 center, float radius,
                float lineThick = 0.0f);
        void
        bar(
                Surface *surface, Rect rect, float value, float max_value, rl::Color color,
                rl::Color bg_color, float radius = 0.0f);
        void
        line(
                Surface *surface, rl::Color color, math::Vector2 start, math::Vector2 end,
                float width = 1.0f);
        void
        lines(
                Surface *surface, rl::Color color, bool closed,
                const std::vector<math::Vector2> &points, float width = 1.0f);
    } // namespace draw

#ifdef WITH_TMX
    namespace tmx
    {
        // World Position, Graphics ID
        struct TileInfo
        {
            math::Vector2 position{}; // position on screen (x*tileSize, y*tileSize)
            unsigned int gid{};
        };

        // get the tile image from the tileset
        rl::Texture2D *GetTMXTileTexture(const rl::tmx_tile *tile, Rect *atlas_rect);
        // get a vector with tile info (position on the layer and surface image)
        std::vector<TileInfo> GetTMXTiles(const rl::tmx_map *map, const rl::tmx_layer *layer);
        // merges all tiles into one single surface image
        Surface GetTMXLayerSurface(const rl::tmx_map *map, const rl::tmx_layer *layer);
        math::Vector2 GetTMXObjPosition(const rl::tmx_object *object);
        // Load all tmx in a folder
        std::unordered_map<std::string, rl::tmx_map *> LoadTMXMaps(const char *path);
        std::unordered_map<unsigned int, Surface> GetTMXSurfaces(const rl::tmx_map *map);
    } // namespace tmx
#endif // WITH_TMX

    namespace sprite
    {
        class Sprite;

        // Manages multiple sprites at once
        class Group
        {
        public:

            Group() = default;
            Group(const Group &other) = delete;
            Group &operator=(const Group &other) = delete;
            Group(Group &&other) noexcept;
            Group &operator=(Group &&other) noexcept;
            virtual ~Group();

            // Draw all sprites into surface
            virtual void Draw(Surface *surface);
            // Updates all sprites
            virtual void Update(float deltaTime);
            // Removes all sprites from Group
            virtual void empty();
            // Removes a list of sprites from this group (if they are part of this group)
            virtual void remove(const std::vector<Sprite *> &to_remove_sprites);
            // Removes a Sprite from this group if it is in this group
            virtual void remove(Sprite *to_remove_sprite);
            // Adds a list of sprites to this group
            virtual void add(const std::vector<Sprite *> &to_add_sprites);
            // Adds a Sprite to this group
            virtual void add(Sprite *to_add_sprite);
            // Check if all sprites are in group
            virtual bool has(const std::vector<Sprite *> &check_sprites) const;
            // Check if sprite is in group
            virtual bool has(Sprite *check_sprite) const;
            // Returns a copy of vector sprites
            virtual std::vector<Sprite *> Sprites();
            // reserve memory for inner vector
            virtual void reserve(size_t size);

        protected:

            std::unordered_map<Sprite *, Sprite *> sprites{};

        private:

            std::vector<Sprite *> result;
        };

        class OrderedUpdates : public Group
        {
        public:

            OrderedUpdates() = default;
            OrderedUpdates(const OrderedUpdates &other) = delete;
            OrderedUpdates &operator=(const OrderedUpdates &other) = delete;
            OrderedUpdates(OrderedUpdates &&other) noexcept;
            OrderedUpdates &operator=(OrderedUpdates &&other) noexcept;
            ~OrderedUpdates() override;

            void Draw(Surface *surface) override;
            void Update(float deltaTime) override;
            void empty() override;
            void remove(const std::vector<Sprite *> &to_remove_sprites) override;
            void remove(Sprite *to_remove_sprite) override;
            void add(const std::vector<Sprite *> &to_add_sprites) override;
            void add(Sprite *to_add_sprite) override;
            bool has(const std::vector<Sprite *> &check_sprites) const override;
            bool has(Sprite *check_sprite) const override;
            std::vector<Sprite *> Sprites() override;
            void reserve(size_t size) override;
            Sprite *pop();

        protected:

            std::vector<Sprite *> sprites{};
        };

        class Sprite
        {
        public:

            Sprite() = default;
            Sprite(const Sprite &other) = delete;
            Sprite &operator=(const Sprite &other) = delete;
            Sprite(Sprite &&other) noexcept;
            Sprite &operator=(Sprite &&other) noexcept;

            virtual ~Sprite();

            // add this sprite to passed group
            void add(Group *to_add_group);
            // add this sprite to all groups
            void add(const std::vector<Group *> &to_add_groups);
            // remove group from this sprite
            void remove(Group *to_remove_group);
            // remove all groups from this sprite
            void remove(const std::vector<Group *> &to_remove_groups);
            // Returns const ref of this sprite groups
            std::vector<Group *> Groups();

            virtual void Update(float deltaTime)
            {
            };
            // removes this sprite from all its group.
            virtual void Kill();

            int z = 0; // in 2D games, used to sort the drawing order

            Rect rect{}; // world position
            Surface *image = nullptr;

        protected:

            // TODO : create Sprite(Group *group)
            // explicit Sprite(Group *to_add_group);
            // explicit Sprite(const std::vector<Group *> &groups);

            std::unordered_map<Group *, Group *> groups{}; // groups that this sprite is in
        private:

            bool has(Group *check_group) const;
            std::vector<Group *> result;
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
                const Sprite *sprite, Group *group, bool dokill,
                const std::function<bool(const Sprite *left, const Sprite *right)> &collided =
                        collide_rect);

        // Tests if Sprite collides with any sprite in group, returns the first sprite in
        // group that collides
        Sprite *spritecollideany(
                const Sprite *sprite, Group *group,
                const std::function<bool(const Sprite *left, const Sprite *right)> &collided =
                        collide_rect);
    } // namespace sprite

    // Remains active for certain duration, can repeat once it is done, can autostart
    // and calls a func at the end. Remember to call Update() on every frame
    class Timer
    {
    public:

        // Default constructor
        Timer() = default;
        // Parameterized constructor
        explicit Timer(
                float duration, bool repeat = false, bool autostart = false,
                const std::function<void()> &func = nullptr);
        void Activate();
        void Deactivate();
        void Update();
        bool active{};
        float duration{};
        std::function<void()> func{};

    private:

        bool repeat{};
        bool autostart{};
        double start_time{};
    };

    namespace display
    {
        Surface &SetMode(int width, int height);
        void SetCaption(const char *title);
        Surface &GetSurface();
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

            Mask() = default;
            Mask(unsigned int width, unsigned int height, bool fill = false);
            Mask(const Mask &other) = delete;
            Mask &operator=(const Mask &other) = delete;
            Mask(Mask &&other) noexcept;
            Mask &operator=(Mask &&other) noexcept;

            ~Mask();
            [[nodiscard]] Surface ToSurface() const;
            [[nodiscard]] Frames ToFrames(int rows, int cols) const;

            rl::Image image{};
            Rect atlas_rect{};
        };

        Mask FromSurface(const Surface *surface, unsigned char threshold = 127);
        Mask FromSurface(const Frames *frames, unsigned char threshold = 127);
    } // namespace mask

    namespace font
    {
        class Font
        {
        public:

            // Default Font, size 1
            explicit Font(float font_size = 1);
            // Load font from file
            Font(const char *file, float font_size);
            // Raylib Font
            Font(rl::Font font, float font_size);
            Font(const Font &other) = delete;
            Font &operator=(const Font &other) = delete;
            Font(Font &&other) noexcept;
            Font &operator=(Font &&other) noexcept;

            ~Font();
            // Creates a Text surface from this Font. Make sure to delete it.
            // If passed padding_width or padding_height, surface dimensions will be added
            // (textsize + (width,height))
            Surface
            render(
                    const char *text, rl::Color color, float spacing = 1, rl::Color bg = rl::BLANK,
                    float padding_width = 0, float padding_height = 0) const;
            math::Vector2 size(const char *text) const;

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
            Sound(const Sound &other) = delete;
            Sound &operator=(const Sound &other) = delete;
            Sound(Sound &&other) noexcept;
            Sound &operator=(Sound &&other) noexcept;
            ~Sound();

            void Play() const;
            void Stop() const;
            void SetVolume(float value) const;
            [[nodiscard]] const std::string &GetFilename() const;

            union audio
            {
                rl::Sound sound;
                rl::Music music;
            } audio{};

        private:

            bool isMusic{};
            std::string file{};
        };
    } // namespace mixer

    namespace transform
    {
        Surface Flip(const Surface *surface, bool flip_x, bool flip_y);
        Frames Flip(const Frames *frames, bool flip_x, bool flip_y);
        Surface GrayScale(const Surface *surface);
        Surface Scale(const Surface *surface, math::Vector2 size);
        Surface Scale2x(const Surface *surface);
    } // namespace transform

} // namespace rg

rg::math::Vector2 operator+(const rg::math::Vector2 &lhs, const rg::math::Vector2 &rhs);
rg::math::Vector2 operator-(const rg::math::Vector2 &lhs, const rg::math::Vector2 &rhs);
rg::math::Vector2 &operator+=(rg::math::Vector2 &lhs, const rg::math::Vector2 &rhs);
rg::math::Vector2 &operator-=(rg::math::Vector2 &lhs, const rg::math::Vector2 &rhs);
rg::math::Vector2 operator*(const rg::math::Vector2 &lhs, float scale);
rg::math::Vector2 &operator*=(rg::math::Vector2 &lhs, float scale);

bool operator!=(const rg::math::Vector3uc &lhs, const rg::math::Vector3uc &rhs);
bool operator!=(const rg::math::Vector3uc &lhs, const rl::Vector3 &rhs);
