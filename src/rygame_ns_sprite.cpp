#include "rygame.hpp"


bool rg::sprite::collide_rect(
        const std::shared_ptr<Sprite> &left, const std::shared_ptr<Sprite> &right)
{
    return CheckCollisionRecs(left->rect.rectangle, right->rect.rectangle);
}

rg::sprite::collide_rect_ratio::collide_rect_ratio(const float ratio) : ratio(ratio)
{}

bool rg::sprite::collide_rect_ratio::operator()(
        const std::shared_ptr<Sprite> left, const std::shared_ptr<Sprite> right) const
{
    Rect leftrect = left->rect;
    Rect rightrect = right->rect;

    leftrect.scale_by_ip(ratio);
    rightrect.scale_by_ip(ratio);

    return collide_rect(left, right);
}

std::vector<std::shared_ptr<rg::sprite::Sprite>> rg::sprite::spritecollide(
        const std::shared_ptr<Sprite> &sprite, const Group *group, const bool dokill,
        const std::function<bool(std::shared_ptr<Sprite> left, std::shared_ptr<Sprite> right)>
                &collided)
{
    std::vector<std::shared_ptr<Sprite>> result;
    for (const auto &other_sprite: group->Sprites())
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

std::shared_ptr<rg::sprite::Sprite> rg::sprite::spritecollideany(
        const std::shared_ptr<Sprite> &sprite, const Group *group,
        const std::function<bool(std::shared_ptr<Sprite> left, std::shared_ptr<Sprite> right)>
                &collided)
{
    for (auto other_sprite: group->Sprites())
    {
        if (collided(sprite, other_sprite))
        {
            return other_sprite;
        }
    }
    return nullptr;
}
