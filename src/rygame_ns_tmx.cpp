#include "rygame.hpp"


#ifdef WITH_TMX
rl::Texture2D *rg::tmx::GetTMXTileTexture(const rl::tmx_tile *tile, Rect *atlas_rect)
{
    const rl::tmx_image *im = tile->image;
    rl::Texture2D *map_texture = nullptr;

    atlas_rect->x = tile->ul_x;
    atlas_rect->y = tile->ul_y;
    atlas_rect->width = tile->width;
    atlas_rect->height = -(float) tile->height;

    if (im && im->resource_image)
    {
        map_texture = (rl::Texture2D *) im->resource_image;
    }
    else if (tile->tileset->image->resource_image)
    {
        map_texture = (rl::Texture2D *) tile->tileset->image->resource_image;
    }

    return map_texture;
}

std::vector<rg::tmx::TileInfo>
rg::tmx::GetTMXTiles(const rl::tmx_map *map, const rl::tmx_layer *layer)
{
    std::vector<TileInfo> tiles{};
    tiles.reserve(map->height * map->width);
    for (unsigned int y = 0; y < map->height; y++)
    {
        for (unsigned int x = 0; x < map->width; x++)
        {
            const unsigned int baseGid = layer->content.gids[y * map->width + x];
            const unsigned int gid = baseGid & TMX_FLIP_BITS_REMOVAL;
            if (map->tiles[gid])
            {
                const rl::tmx_tileset *ts = map->tiles[gid]->tileset;
                Rect atlas_rect{};
                auto *tileTexture = GetTMXTileTexture(map->tiles[gid], &atlas_rect);
                const math::Vector2 pos = {(float) x * ts->tile_width, (float) y * ts->tile_height};
                TileInfo tile_info = {pos, tileTexture, atlas_rect};
                tiles.push_back(tile_info);
            }
        }
    }
    return tiles;
}

rg::Surface_Ptr
rg::tmx::GetTMXLayerSurface(const rl::tmx_map *map, const rl::tmx_layer *layer)
{
    const auto surface = std::make_shared<Surface>(
            (int) (map->width * map->tile_width), (int) (map->height * map->tile_height));
    surface->Fill(rl::BLANK);
    // GetTMXTiles will return many Texture*, but we don't need to unload them here, only
    // at rg::UnloadTMX
    const std::vector<TileInfo> tiles = GetTMXTiles(map, layer);
    for (const auto &[position, texture, atlas_rect]: tiles)
    {
        surface->Blit(*texture, position, atlas_rect);
    }
    return surface;
}

rg::math::Vector2 rg::tmx::GetTMXObjPosition(const rl::tmx_object *object)
{
    float x = 0, y = 0;
    switch (object->obj_type)
    {
        case rl::OT_NONE:
        case rl::OT_POLYGON:
        case rl::OT_POLYLINE:
        case rl::OT_ELLIPSE:
        case rl::OT_TEXT:
            break;
        case rl::OT_SQUARE:
        case rl::OT_POINT:
            x = object->x;
            y = object->y;
            break;
        case rl::OT_TILE:
            x = object->x;
            y = object->y - object->height;
            break;
    }
    return math::Vector2{x, y};
}

std::map<std::string, rl::tmx_map *> rg::tmx::LoadTMXMaps(const char *path)
{
    std::map<std::string, rl::tmx_map *> result;
    for (const auto &dirEntry: std::filesystem::recursive_directory_iterator(path))
    {
        auto filename = dirEntry.path().stem().string();
        auto entryPath = dirEntry.path().string();
        result[filename] = rl::LoadTMX(entryPath.c_str());
    }
    return result;
}

#endif // WITH_TMX
