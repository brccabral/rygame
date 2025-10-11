#include <rygame.hpp>


rg::Surface rg::image::Load(const char *path)
{
    // we Blit the loaded texture so it is considered local and unloaded in ~Surface()
    auto loaded_texture = LoadTextureSafe(path);
    if (!loaded_texture.id)
    {
        throw std::runtime_error("Failed to load " + std::string(path));
    }
    auto result = Surface(loaded_texture.width, loaded_texture.height);
    auto tempSurf = Surface(&loaded_texture);
    result.Fill(rl::BLANK);
    result.Blit(&tempSurf, rg::math::Vector2<float>{});
    result.Draw();
    UnloadTextureSafe(loaded_texture);
    return result;
}

std::vector<rg::Surface> rg::image::LoadFolderList(const char *path)
{
    std::vector<Surface> surfaces;
    std::vector<std::string> files_path;
    for (const auto &dirEntry: std::filesystem::directory_iterator(path))
    {
        files_path.push_back(dirEntry.path().string());
    }

    std::ranges::sort(files_path);

    for (const auto &filepath: files_path)
    {
        surfaces.push_back(Load(filepath.c_str()));
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
        surfaces[filename] = Load(entryPath.c_str());
    }
    return surfaces;
}
