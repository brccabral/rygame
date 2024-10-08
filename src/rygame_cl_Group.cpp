#include "rygame.hpp"


void rg::sprite::Group::Draw(const Surface_Ptr &surface)
{
    for (const auto &sprite: sprites)
    {
        surface->Blit(sprite->image, sprite->rect);
    }
}

void rg::sprite::Group::Update(const float deltaTime) const
{
    for (const auto &sprite: Sprites())
    {
        sprite->Update(deltaTime);
    }
}

void rg::sprite::Group::empty()
{
    for (const auto &sprite: Sprites())
    {
        sprite->remove(this);
    }
    sprites.clear();
}

void rg::sprite::Group::remove(const std::vector<Sprite_Ptr> &to_remove_sprites)
{
    for (const auto &sprite: to_remove_sprites)
    {
        remove(sprite);
    }
}

void rg::sprite::Group::remove( // NOLINT(*-no-recursion) - the recursion is broken with has()
        const Sprite_Ptr &to_remove_sprite)
{
    if (has(to_remove_sprite))
    {
        sprites.erase(std::remove(sprites.begin(), sprites.end(), to_remove_sprite), sprites.end());
        to_remove_sprite->remove(this);
    }
}

void rg::sprite::Group::add(const std::vector<Sprite_Ptr> &to_add_sprites)
{
    for (const auto &sprite: to_add_sprites)
    {
        add(sprite);
    }
}

void rg::sprite::Group::add( // NOLINT(*-no-recursion) - the recursion is broken with has()
        const Sprite_Ptr &to_add_sprite)
{
    if (!has(to_add_sprite))
    {
        sprites.push_back(to_add_sprite);
        to_add_sprite->add(this);
    }
}

bool rg::sprite::Group::has(const std::vector<Sprite_Ptr> &check_sprites)
{
    for (const auto &sprite: check_sprites)
    {
        if (!has(sprite))
        {
            return false;
        }
    }
    return true;
}

bool rg::sprite::Group::has(const Sprite_Ptr &check_sprite)
{
    return std::find(sprites.begin(), sprites.end(), check_sprite) != sprites.end();
}

std::vector<rg::sprite::Sprite_Ptr> rg::sprite::Group::Sprites() const
{
    return sprites;
}
