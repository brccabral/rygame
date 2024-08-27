#include "rygame.hpp"


rg::Surface_Ptr rg::image::Load(const char *path)
{
    // we Blit the loaded texture so it is considered local and unloaded in ~Surface()
    const rl::Texture2D loaded_texture = LoadTextureSafe(path);
    const auto surface = std::make_shared<Surface>(loaded_texture.width, loaded_texture.height);
    surface->Fill(rl::BLANK);
    surface->Blit(
            loaded_texture, {},
            {0, 0, (float) loaded_texture.width, -(float) loaded_texture.height});
    UnloadTextureSafe(loaded_texture);
    return surface;
}

std::vector<rg::Surface_Ptr> rg::image::LoadFolderList(const char *path)
{
    std::vector<Surface_Ptr> surfaces;
    for (const auto &dirEntry: std::filesystem::recursive_directory_iterator(path))
    {
        auto entryPath = dirEntry.path().string();
        surfaces.push_back(Load(entryPath.c_str()));
    }
    return surfaces;
}

std::map<std::string, rg::Surface_Ptr> rg::image::LoadFolderDict(const char *path)
{
    std::map<std::string, Surface_Ptr> surfaces;
    for (const auto &dirEntry: std::filesystem::recursive_directory_iterator(path))
    {
        auto filename = dirEntry.path().stem().string();
        auto entryPath = dirEntry.path().string();
        // ReSharper disable once CppDFAMemoryLeak
        surfaces[filename] = Load(entryPath.c_str());
    }
    return surfaces;
}

std::vector<rg::Surface_Ptr> rg::image::ImportFolder(const char *path)
{
    std::vector<Surface_Ptr> surfaces;
    for (const auto &dirEntry: std::filesystem::recursive_directory_iterator(path))
    {
        auto entryPath = dirEntry.path().string();
        surfaces.push_back(Load(entryPath.c_str()));
    }
    return surfaces;
}

std::map<std::string, rg::Surface_Ptr> rg::image::ImportFolderDict(const char *path)
{
    std::map<std::string, Surface_Ptr> result;
    for (const auto &dirEntry: std::filesystem::recursive_directory_iterator(path))
    {
        auto entryPath = dirEntry.path().string();
        auto filename = dirEntry.path().stem().string();
        result[filename] = Load(entryPath.c_str());
    }
    return result;
}
