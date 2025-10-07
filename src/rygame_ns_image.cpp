#include <rygame.hpp>


rg::Surface rg::image::Load(const char *path)
{
    // we Blit the loaded texture so it is considered local and unloaded in ~Surface()
    const rl::Texture2D loaded_texture = LoadTextureSafe(path);
    auto result = Surface(loaded_texture.width, loaded_texture.height);
    result.ApplyTexture(loaded_texture);
    result.atlas_rect.height *= -1;
    return result;
}

std::vector<rg::Surface> rg::image::LoadFolderList(const char *path)
{
    std::vector<Surface> surfaces;
    for (const auto &dirEntry: std::filesystem::recursive_directory_iterator(path))
    {
        auto entryPath = dirEntry.path().string();
        surfaces.push_back(Load(entryPath.c_str()));
    }
    return surfaces;
}

std::unordered_map<std::string, rg::Surface> rg::image::LoadFolderDict(const char *path)
{
    std::unordered_map<std::string, Surface> surfaces;
    for (const auto &dirEntry: std::filesystem::recursive_directory_iterator(path))
    {
        auto filename = dirEntry.path().stem().string();
        auto entryPath = dirEntry.path().string();
        // ReSharper disable once CppDFAMemoryLeak
        surfaces[filename] = Load(entryPath.c_str());
    }
    return surfaces;
}

std::vector<rg::Surface> rg::image::ImportFolder(const char *path)
{
    std::vector<Surface> surfaces;
    for (const auto &dirEntry: std::filesystem::recursive_directory_iterator(path))
    {
        auto entryPath = dirEntry.path().string();
        surfaces.push_back(Load(entryPath.c_str()));
    }
    return surfaces;
}

std::unordered_map<std::string, rg::Surface> rg::image::ImportFolderDict(const char *path)
{
    std::unordered_map<std::string, Surface> result;
    for (const auto &dirEntry: std::filesystem::recursive_directory_iterator(path))
    {
        auto entryPath = dirEntry.path().string();
        auto filename = dirEntry.path().stem().string();
        result[filename] = Load(entryPath.c_str());
    }
    return result;
}
