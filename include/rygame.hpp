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
    Init(int logLevel = rl::LOG_WARNING, unsigned int config_flags = 0,
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
        // GCC warns about Anonymous Struct
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
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
#pragma GCC diagnostic pop

        std::uniform_real_distribution<float> random_uniform_dist(float min, float max);
        float random_uniform(std::uniform_real_distribution<float> dist);
        float get_random_uniform(float min, float max);
        float clamp(float value, float min, float max);
    } // namespace math

    // GCC warns about Anonymous Struct
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
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
#pragma GCC diagnostic pop

    // GCC warns about Anonymous Struct
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
    typedef union Rect
    {
        struct
        {
            math::Vector2 pos, size;
        };

        rl::Rectangle rectangle;

        struct
        {
            float x, y, width, height;
        };

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
        Line clipline(Line line);
        // If passed line (from start to end) crosses the rect, returns a new line that is just
        // inside the rect. If passed line is outside, returns an empty line {}
        Line clipline(math::Vector2 start, math::Vector2 end);
        // If passed line (from x1,y1 to x2,y2) crosses the rect, returns a new line that is just
        // inside the rect. If passed line is outside, returns an empty line {}
        Line clipline(float x1, float y1, float x2, float y2);
    } Rect;
#pragma GCC diagnostic pop

    class Surface : public std::enable_shared_from_this<Surface>
    {
    public:

        // Surface cannot be allocated in Heap
        void *operator new(size_t) = delete;

        Surface(int width, int height);
        explicit Surface(math::Vector2 size);
        explicit Surface(rl::Texture2D *texture, Rect atlas = {});

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
        Blit(const std::shared_ptr<Surface> &incoming, Rect offset,
             rl::BlendMode blend_mode = rl::BLEND_ALPHA);
        // Blit incoming Surface* into this.
        void
        Blit(const std::shared_ptr<Surface> &incoming, math::Vector2 offset,
             rl::BlendMode blend_mode = rl::BLEND_ALPHA);
        // Blit incoming Texture2D into surface*.
        void
        Blit(const rl::Texture2D &incoming_texture, math::Vector2 offset, Rect area = {},
             rl::BlendMode blend_mode = rl::BLEND_ALPHA, rl::Color tint = rl::WHITE);
        // Blit many surfaces into this. `blit_sequence` is a vector of pairs of incoming
        // surface* and offset
        void
        Blits(const std::vector<std::pair<std::shared_ptr<Surface>, math::Vector2>> &blit_sequence,
              rl::BlendMode blend_mode = rl::BLEND_ALPHA);
        // Creates a new Surface*.
        // Make sure to delete it
        [[nodiscard]] std::shared_ptr<Surface> convert(rl::PixelFormat format) const;
        [[nodiscard]] std::shared_ptr<Surface> copy() const;
        // Returns the atlas size
        [[nodiscard]] Rect GetRect() const;

        // Returns a different shared_ptr<Surface>, but it shares same image
        // as this one. SubSurface will have this as parent (GetParent, GetAbsParent).
        virtual std::shared_ptr<Surface> SubSurface(Rect rect);
        std::shared_ptr<Surface> GetParent();
        std::shared_ptr<Surface> GetAbsParent();

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

        std::shared_ptr<Surface> parent = nullptr;
        math::Vector2 offset{};
        float flip_atlas_height = 1; // 1 or -1 (Frames)

        rl::Color tint{255, 255, 255, 255};
    };

    class Frames : public Surface
    {
    public:

        // Width/Height is the total size of the image
        // Rows/Cols will create atlas vector with N=rows*cols, each N of
        // size (Width/Cols, Height/Rows)
        Frames(int width, int height, int rows, int cols);
        Frames(const std::shared_ptr<Surface> &surface, int rows, int cols);

        // Set current atlas rect. Default to first frame.
        // Value is moduled with frame length in case it is greater than frames size.
        void SetAtlas(int frame_index = 0);
        // Merge a list of Surfaces. Assumes all surfaces are same size.
        // Caller must delete returned Frame*
        static std::shared_ptr<Frames>
        Merge(const std::vector<std::shared_ptr<Surface>> &surfaces, int rows, int cols);
        // Load an image and create frames for it
        static std::shared_ptr<Frames> Load(const char *file, int rows, int cols);
        void SetColorKey(rl::Color color) override;

        std::shared_ptr<Surface> SubSurface(Rect rect) override
        {
            throw;
        };
        // Returns a different shared_ptr<Frames>, but it shares same image
        // as this one. SubSurface will have this as parent (GetParent, GetAbsParent).
        // SubSurface will have its frames updated.
        std::shared_ptr<Frames> SubFrames(Rect rect);

        int current_frame_index{};
        std::vector<Rect> frames{};
        int rows{};
        int cols{};

    private:

        void CreateFrames(int width, int height, int rows, int cols);
    };

    namespace image
    {
        // Load a file into a new Surface*
        // Make sure to delete it
        std::shared_ptr<Surface> Load(const char *path);
        // Loads all files in a folder and returns a vector<> of new Surface*
        // Make sure to delete them
        std::vector<std::shared_ptr<Surface>> LoadFolderList(const char *path);
        // Loads all files in a folder and returns a map<> (dictionary) of new Surface*
        // where the key is the filename
        // Make sure to delete them
        std::map<std::string, std::shared_ptr<Surface>> LoadFolderDict(const char *path);
        // Walk a folder path and loads all images
        // Returns a vector of Surface*
        // The caller must delete Surface*
        std::vector<std::shared_ptr<Surface>> ImportFolder(const char *path);
        // Walk a folder path and loads all images
        // Returns a map where the key is filename and values are Surface*
        // The caller must delete Surface*
        std::map<std::string, std::shared_ptr<Surface>> ImportFolderDict(const char *path);
    } // namespace image

    namespace draw
    {
        void
        rect(const std::shared_ptr<Surface> &surface, rl::Color color, Rect rect,
             float lineThick = 0.0f, float radius = 0.0f, bool topLeft = true, bool topRight = true,
             bool bottomLeft = true, bool bottomRight = true);
        void
        circle(const std::shared_ptr<Surface> &surface, rl::Color color, math::Vector2 center,
               float radius, float lineThick = 0.0f);
        void
        bar(const std::shared_ptr<Surface> &surface, Rect rect, float value, float max_value,
            rl::Color color, rl::Color bg_color, float radius = 0.0f);
        void
        line(const std::shared_ptr<Surface> &surface, rl::Color color, math::Vector2 start,
             math::Vector2 end, float width = 1.0f);
        void
        lines(const std::shared_ptr<Surface> &surface, rl::Color color, bool closed,
              const std::vector<math::Vector2> &points, float width = 1.0f);
    } // namespace draw

#ifdef WITH_TMX
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
        std::shared_ptr<Surface>
        GetTMXLayerSurface(const rl::tmx_map *map, const rl::tmx_layer *layer);
        math::Vector2 GetTMXObjPosition(const rl::tmx_object *object);
        // Load all tmx in a folder
        std::map<std::string, rl::tmx_map *> LoadTMXMaps(const char *path);
    } // namespace tmx
#endif // WITH_TMX

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
            virtual void Draw(const std::shared_ptr<Surface> &surface);
            // Updates all sprites
            void Update(float deltaTime) const;
            // Removes all sprites from Group
            void empty();
            // Removes a list of sprites from this group (if they are part of this group)
            void remove(const std::vector<std::shared_ptr<Sprite>> &to_remove_sprites);
            // Removes a Sprite from this group if it is in this group
            void remove(const std::shared_ptr<Sprite> &to_remove_sprite);
            // Adds a list of sprites to this group
            void add(const std::vector<std::shared_ptr<Sprite>> &to_add_sprites);
            // Adds a Sprite to this group
            void add(const std::shared_ptr<Sprite> &to_add_sprite);
            // Check if all sprites are in group
            bool has(const std::vector<std::shared_ptr<Sprite>> &check_sprites);
            // Check if sprite is in group
            bool has(const std::shared_ptr<Sprite> &check_sprite);
            // Returns a copy of vector sprites
            [[nodiscard]] std::vector<std::shared_ptr<Sprite>> Sprites() const;


        protected:

            std::vector<std::shared_ptr<Sprite>> sprites{};
        };

        class Sprite : public std::enable_shared_from_this<Sprite>
        {
        public:

            // Sprite cannot be allocated in Heap
            void *operator new(size_t) = delete;

            Sprite();

            virtual ~Sprite() = default;

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
            virtual std::shared_ptr<Sprite> Kill();

            int z = 0; // in 2D games, used to sort the drawing order

            Rect rect{}; // world position
            std::shared_ptr<Surface> image;

        protected:

            // !!!!! Can't have these constructors because it can't call "shared_from_this()" before
            // object has actually been created
            // explicit Sprite(Group *to_add_group);
            // explicit Sprite(const std::vector<Group *> &groups);

            std::vector<Group *> groups{}; // groups that this sprite is in
        private:

            bool has(const Group *check_group);
            // Leave groups that are not the passed one
            virtual void LeaveOtherGroups(const Group *not_leave_group);
            // leave all groups
            void LeaveAllGroups();
        };

        bool
        collide_rect(const std::shared_ptr<Sprite> &left, const std::shared_ptr<Sprite> &right);

        class CollideCallable
        {
        public:

            CollideCallable() = default;

            virtual bool
            operator()(std::shared_ptr<Sprite> left, std::shared_ptr<Sprite> right) const = 0;

        protected:

            virtual ~CollideCallable() = default;
        };

        class collide_rect_ratio : public CollideCallable
        {
        public:

            explicit collide_rect_ratio(float ratio);
            bool
            operator()(std::shared_ptr<Sprite> left, std::shared_ptr<Sprite> right) const override;

        private:

            float ratio;
        };


        // Returns a list of all sprites in the group that collides with the sprite
        // If dokill is true, all collided sprites are removed from group
        std::vector<std::shared_ptr<Sprite>> spritecollide(
                const std::shared_ptr<Sprite> &sprite, const Group *group, bool dokill,
                const std::function<bool(
                        std::shared_ptr<Sprite> left, std::shared_ptr<Sprite> right)> &collided =
                        collide_rect);

        // Tests if Sprite collides with any sprite in group, returns the first sprite in
        // group that collides
        std::shared_ptr<Sprite> spritecollideany(
                const std::shared_ptr<Sprite> &sprite, const Group *group,
                const std::function<bool(
                        std::shared_ptr<Sprite> left, std::shared_ptr<Sprite> right)> &collided =
                        collide_rect);
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
        std::shared_ptr<Surface> SetMode(int width, int height);
        void SetCaption(const char *title);
        std::shared_ptr<Surface> GetSurface();
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
            [[nodiscard]] std::shared_ptr<Surface> ToSurface() const;
            [[nodiscard]] std::shared_ptr<Frames> ToFrames(int rows, int cols) const;

            rl::Image image{};
            Rect atlas_rect{};
        };

        Mask FromSurface(const std::shared_ptr<Surface> &surface, unsigned char threshold = 127);
        Mask FromSurface(const std::shared_ptr<Frames> &frames, unsigned char threshold = 127);
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
            ~Font();
            // Creates a Text surface from this Font. Make sure to delete it.
            // If passed padding_width or padding_height, surface dimensions will be added
            // (textsize + (width,height))
            std::shared_ptr<Surface>
            render(const char *text, rl::Color color, float spacing = 1, rl::Color bg = rl::BLANK,
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

    namespace transform
    {
        std::shared_ptr<Surface>
        Flip(const std::shared_ptr<Surface> &surface, bool flip_x, bool flip_y);
        std::shared_ptr<Frames>
        Flip(const std::shared_ptr<Frames> &frames, bool flip_x, bool flip_y);
        std::shared_ptr<Surface> GrayScale(const std::shared_ptr<Surface> &surface);
        std::shared_ptr<Surface> Scale(const std::shared_ptr<Surface> &surface, math::Vector2 size);
        std::shared_ptr<Surface> Scale2x(const std::shared_ptr<Surface> &surface);
    } // namespace transform

} // namespace rg

rg::math::Vector2 operator+(const rg::math::Vector2 &lhs, const rg::math::Vector2 &rhs);
rg::math::Vector2 operator-(const rg::math::Vector2 &lhs, const rg::math::Vector2 &rhs);
rg::math::Vector2 &operator+=(rg::math::Vector2 &lhs, const rg::math::Vector2 &rhs);
rg::math::Vector2 &operator-=(rg::math::Vector2 &lhs, const rg::math::Vector2 &rhs);
rg::math::Vector2 operator*(const rg::math::Vector2 &lhs, float scale);
rg::math::Vector2 &operator*=(rg::math::Vector2 &lhs, float scale);
