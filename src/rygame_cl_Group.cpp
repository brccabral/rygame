#include <algorithm>
#include "rygame.hpp"


void rg::sprite::Group::Draw(Surface *surface)
{
    for (const auto *sprite: sprites)
    {
        surface->Blit(sprite->image, sprite->rect);
    }
}

void rg::sprite::Group::Update(const float deltaTime) const
{
    for (auto *sprite: Sprites())
    {
        sprite->Update(deltaTime);
    }
}

void rg::sprite::Group::empty()
{
    for (auto *sprite: Sprites())
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

std::vector<rg::sprite::Sprite *> rg::sprite::Group::Sprites() const
{
    return sprites;
}
