#include "rygame.hpp"


// !!!!! Can't have these constructors because it can't call "this" before
// object has actually been created
// rg::sprite::Sprite::Sprite(Group *to_add_group)
// {
//     if (to_add_group)
//     {
//         add(to_add_group);
//     }
// }
//
// rg::sprite::Sprite::Sprite(const std::vector<Group *> &groups)
// {
//     add(groups);
// }

rg::sprite::Sprite::Sprite(Sprite &&other) noexcept
    : Sprite()
{
    *this = std::move(other);
}

rg::sprite::Sprite &rg::sprite::Sprite::operator=(Sprite &&other) noexcept
{
    if (this != &other)
    {
        z = other.z;
        rect = other.rect;
        image = other.image;
        // need to tell groups that there is a new sprite
        Kill();
        groups.reserve(other.groups.size());
        result.resize(other.result.size());
        add(other.Groups());
        other.image = nullptr;
    }
    return *this;
}

rg::sprite::Sprite::~Sprite()
{
    Sprite::Kill();
}

void rg::sprite::Sprite::add(
        // NOLINT(*-no-recursion) - the recursion is broken with has()
        Group *to_add_group)
{
    if (to_add_group)
    {
        if (!has(to_add_group))
        {
            groups.emplace(to_add_group, to_add_group);
            to_add_group->add(this);
        }
    }
}

void rg::sprite::Sprite::add(const std::vector<Group *> &to_add_groups)
{
    for (auto *sprite_group: to_add_groups)
    {
        add(sprite_group);
    }
}

void rg::sprite::Sprite::remove(
        // NOLINT(*-no-recursion) - the recursion is broken with has()
        Group *to_remove_group)
{
    if (has(to_remove_group))
    {
        groups.erase(to_remove_group);
        to_remove_group->remove(this);
    }
}

void rg::sprite::Sprite::remove(const std::vector<Group *> &to_remove_groups)
{
    for (auto *to_remove_group: to_remove_groups)
    {
        remove(to_remove_group);
    }
}

std::vector<rg::sprite::Group *> rg::sprite::Sprite::Groups()
{
    // it has to return a vector copy because in a for-loop
    // user might call sprite.Kill and it will invalidate groups
    result.clear();
    result.reserve(groups.size());
    result.insert(
            result.end(), (groups | std::views::keys).begin(), (groups | std::views::keys).end());
    return result;
}

void rg::sprite::Sprite::Kill()
{
    if (groups.empty())
    {
        return;
    }
    // leave all groups
    // need a copy because group->remove() calls erase() which invalidates iterator
    const std::vector<Group *> cpy = Groups();
    for (auto *group: cpy)
    {
        group->remove(this);
    }
    // it doesn't belong to any group
    groups.clear();
}

bool rg::sprite::Sprite::has(Group *check_group) const
{
    return groups.contains(check_group);
}
