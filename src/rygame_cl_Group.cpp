#include <algorithm>
#include "rygame.hpp"


rg::sprite::Group::Group(Group &&other) noexcept
    : Group()
{
    *this = std::move(other);
}

rg::sprite::Group &rg::sprite::Group::operator=(Group &&other) noexcept
{
    if (this != &other)
    {
        // need to tell sprites that there is a new group
        sprites.reserve(other.sprites.capacity());
        add(other.Sprites());
    }
    return *this;
}

rg::sprite::Group::~Group()
{
    empty();
}

void rg::sprite::Group::Draw(Surface *surface)
{
    for (const auto *sprite: sprites)
    {
        surface->Blit(sprite->image, sprite->rect);
    }
}

void rg::sprite::Group::Update(const float deltaTime) const
{
    for (auto *sprite: sprites)
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
    const auto cpy = sprites;
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
        std::erase(sprites, to_remove_sprite);
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
        sprites.push_back(to_add_sprite);
        to_add_sprite->add(this);
    }
}

bool rg::sprite::Group::has(const std::vector<Sprite *> &check_sprites)
{
    for (const auto *sprite: check_sprites)
    {
        if (!has(sprite))
        {
            return false;
        }
    }
    return true;
}

bool rg::sprite::Group::has(const Sprite *check_sprite)
{
    return std::find(sprites.begin(), sprites.end(), check_sprite) != sprites.end();
}

const std::vector<rg::sprite::Sprite *> &rg::sprite::Group::Sprites() const
{
    return sprites;
}

void rg::sprite::Group::reserve(const size_t size)
{
    sprites.reserve(size);
}
