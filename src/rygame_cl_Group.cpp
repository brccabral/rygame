#include <rygame.hpp>


rg::sprite::Group::Group(Group &&other) noexcept
    : Group()
{
    *this = std::move(other);
}

rg::sprite::Group &rg::sprite::Group::operator=(Group &&other) noexcept
{
    if (this != &other)
    {
        empty();
        // need to tell sprites that there is a new group
        sprites.reserve(other.sprites.size());
        add(other.Sprites());
    }
    return *this;
}

rg::sprite::Group::~Group()
{
    Group::empty();
}

void rg::sprite::Group::Draw(Surface *surface)
{
    for (const auto *sprite: sprites | std::views::keys)
    {
        surface->Blit(sprite->image, sprite->rect);
    }
}

void rg::sprite::Group::Update(const float deltaTime)
{
    for (auto *sprite: Sprites())
    {
        sprite->Update(deltaTime);
    }
}

void rg::sprite::Group::empty()
{
    if (sprites.empty())
    {
        return;
    }
    // need a copy because sprite->remove() calls erase() which invalidates iterator
    std::vector<Sprite *> cpy;
    cpy.reserve(sprites.size());
    cpy.insert(
            cpy.end(), (sprites | std::views::keys).begin(), (sprites | std::views::keys).end());
    for (auto *sprite: cpy)
    {
        sprite->remove(this);
    }
    sprites.clear();
}

void rg::sprite::Group::remove(const std::vector<Sprite *> &to_remove_sprites)
{
    for (auto *sprite: to_remove_sprites)
    {
        remove(sprite);
    }
}

void rg::sprite::Group::remove(
        // NOLINT(*-no-recursion) - the recursion is broken with has()
        Sprite *to_remove_sprite)
{
    if (has(to_remove_sprite))
    {
        sprites.erase(to_remove_sprite);
        to_remove_sprite->remove(this);
    }
}

void rg::sprite::Group::add(const std::vector<Sprite *> &to_add_sprites)
{
    for (auto *sprite: to_add_sprites)
    {
        add(sprite);
    }
}

void rg::sprite::Group::add(
        // NOLINT(*-no-recursion) - the recursion is broken with has()
        Sprite *to_add_sprite)
{
    if (!has(to_add_sprite))
    {
        sprites.emplace(to_add_sprite, to_add_sprite);
        to_add_sprite->add(this);
    }
}

bool rg::sprite::Group::has(const std::vector<Sprite *> &check_sprites) const
{
    for (auto *sprite: check_sprites)
    {
        if (!has(sprite))
        {
            return false;
        }
    }
    return true;
}

bool rg::sprite::Group::has(Sprite *check_sprite) const
{
    return sprites.contains(check_sprite);
}

std::vector<rg::sprite::Sprite *> rg::sprite::Group::Sprites()
{
    // it has to return a vector copy because in a for-loop
    // user might call sprite.Kill and it will invalidate groups
    result.clear();
    result.reserve(sprites.size());
    result.insert(
            result.end(), (sprites | std::views::keys).begin(), (sprites | std::views::keys).end());
    return result;
}

void rg::sprite::Group::reserve(const size_t size)
{
    sprites.reserve(size);
}

rg::sprite::OrderedUpdates::OrderedUpdates(OrderedUpdates &&other) noexcept
    : OrderedUpdates()
{
    *this = std::move(other);
}

rg::sprite::OrderedUpdates &rg::sprite::OrderedUpdates::operator=(OrderedUpdates &&other) noexcept
{
    if (this != &other)
    {
        empty();
        // need to tell sprites that there is a new group
        sprites.reserve(other.sprites.size());
        add(other.Sprites());
    }
    return *this;
}

rg::sprite::OrderedUpdates::~OrderedUpdates()
{
    OrderedUpdates::empty();
}

void rg::sprite::OrderedUpdates::Draw(Surface *surface)
{
    for (const auto *sprite: sprites)
    {
        surface->Blit(sprite->image, sprite->rect);
    }
}

void rg::sprite::OrderedUpdates::Update(const float deltaTime)
{
    for (auto *sprite: Sprites())
    {
        sprite->Update(deltaTime);
    }
}

void rg::sprite::OrderedUpdates::empty()
{
    if (sprites.empty())
    {
        return;
    }
    // need a copy because sprite->remove() calls erase() which invalidates iterator
    std::vector<Sprite *> cpy;
    cpy.reserve(sprites.size());
    cpy.insert(cpy.end(), sprites.begin(), sprites.end());
    for (auto *sprite: cpy)
    {
        sprite->remove(this);
    }
    sprites.clear();
}

void rg::sprite::OrderedUpdates::remove(const std::vector<Sprite *> &to_remove_sprites)
{
    for (auto *sprite: to_remove_sprites)
    {
        remove(sprite);
    }
}

void rg::sprite::OrderedUpdates::remove(
        // NOLINT(*-no-recursion) - the recursion is broken with has()
        Sprite *to_remove_sprite)
{
    if (has(to_remove_sprite))
    {
        std::erase(sprites, to_remove_sprite);
        to_remove_sprite->remove(this);
    }
}

void rg::sprite::OrderedUpdates::add(const std::vector<Sprite *> &to_add_sprites)
{
    for (auto *sprite: to_add_sprites)
    {
        add(sprite);
    }
}

void rg::sprite::OrderedUpdates::add(Sprite *to_add_sprite)
{
    if (!has(to_add_sprite))
    {
        sprites.emplace_back(to_add_sprite);
        to_add_sprite->add(this);
    }
}

bool rg::sprite::OrderedUpdates::has(const std::vector<Sprite *> &check_sprites) const
{
    for (auto *sprite: check_sprites)
    {
        if (!has(sprite))
        {
            return false;
        }
    }
    return true;
}

bool rg::sprite::OrderedUpdates::has(Sprite *check_sprite) const
{
    return std::ranges::find(sprites, check_sprite) != sprites.end();
}

std::vector<rg::sprite::Sprite *> rg::sprite::OrderedUpdates::Sprites()
{
    // it has to return a vector copy because in a for-loop
    // user might call sprite.Kill and it will invalidate groups
    return sprites;
}

void rg::sprite::OrderedUpdates::reserve(const size_t size)
{
    sprites.reserve(size);
}

rg::sprite::Sprite *rg::sprite::OrderedUpdates::pop()
{
    Sprite *result = sprites.back();
    remove(result);
    return result;
}

rg::sprite::Sprite *rg::sprite::OrderedUpdates::back() const
{
    return sprites.back();
}

size_t rg::sprite::OrderedUpdates::size() const
{
    return sprites.size();
}

size_t rg::sprite::OrderedUpdates::index(Sprite *sprite) const
{
    return rg::index(sprites, sprite);
}

rg::sprite::Sprite *rg::sprite::OrderedUpdates::operator[](const size_t index) const
{
    return sprites[index];
}
