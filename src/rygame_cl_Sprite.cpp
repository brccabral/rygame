#include "rygame.hpp"


rg::sprite::Sprite::Sprite() = default;

// !!!!! Can't have these constructors because it can't call "shared_from_this()" before
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

void rg::sprite::Sprite::add( // NOLINT(*-no-recursion) - the recursion is broken with has()
        Group *to_add_group)
{
    if (to_add_group)
    {
        if (!has(to_add_group))
        {
            groups.push_back(to_add_group);
            // to_add_group->add(this);
            to_add_group->add(shared_from_this());
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

void rg::sprite::Sprite::remove( // NOLINT(*-no-recursion) - the recursion is broken with has()
        Group *to_remove_group)
{
    if (has(to_remove_group))
    {
        groups.erase(std::remove(groups.begin(), groups.end(), to_remove_group), groups.end());
        to_remove_group->remove(shared_from_this());
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
    return groups;
}

std::shared_ptr<rg::sprite::Sprite> rg::sprite::Sprite::Kill()
{
    // leave all groups
    LeaveAllGroups();
    return shared_from_this();
}

bool rg::sprite::Sprite::has(const Group *check_group)
{
    return std::find(groups.begin(), groups.end(), check_group) != groups.end();
}

void rg::sprite::Sprite::LeaveOtherGroups(const Group *not_leave_group)
{
    for (const auto group: Groups())
    {
        if (group != not_leave_group)
        {
            group->remove(shared_from_this());
        }
    }
}

void rg::sprite::Sprite::LeaveAllGroups() // NOLINT(*-no-recursion) - the recursion does not
                                          // happen because we pass `false`
{
    // leave all groups
    for (const auto group: Groups())
    {
        group->remove(shared_from_this());
    }
    // it doesn't belong to any group
    groups.clear();
}
