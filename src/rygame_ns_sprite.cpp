#include "rygame.hpp"


bool rg::sprite::collide_rect(const Sprite *left, const Sprite *right)
{
    return CheckCollisionRecs(left->rect.rectangle, right->rect.rectangle);
}

rg::sprite::collide_rect_ratio::collide_rect_ratio(const float ratio) : ratio(ratio)
{
}

bool rg::sprite::collide_rect_ratio::operator()(const Sprite *left, const Sprite *right) const
{
    Rect leftrect = left->rect;
    Rect rightrect = right->rect;

    leftrect.scale_by_ip(ratio);
    rightrect.scale_by_ip(ratio);

    return collide_rect(left, right);
}

std::vector<rg::sprite::Sprite *> rg::sprite::spritecollide(
        const Sprite *sprite, const Group *group, const bool dokill,
        const std::function<bool(const Sprite *left, const Sprite *right)> &collided)
{
    std::vector<Sprite *> result;
    for (auto *other_sprite: group->Sprites())
    {
        if (collided(sprite, other_sprite))
        {
            result.push_back(other_sprite);
            if (dokill)
            {
                // just remove from group, don't delete
                // it will be returned in the result
                // if needed, delete it in the vector later
                other_sprite->Kill();
            }
        }
    }
    return result;
}

rg::sprite::Sprite *rg::sprite::spritecollideany(
        const Sprite *sprite, const Group *group,
        const std::function<bool(const Sprite *left, const Sprite *right)> &collided)
{
    for (auto *other_sprite: group->Sprites())
    {
        if (collided(sprite, other_sprite))
        {
            return other_sprite;
        }
    }
    return nullptr;
}
