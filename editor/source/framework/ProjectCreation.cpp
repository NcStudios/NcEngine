#include "ProjectCreation.h"

#include <fstream>

namespace nc::editor
{
    bool IsValidProjectParentDirectory(const std::filesystem::path& parentDirectory)
    {
        auto pathEntry = std::filesystem::directory_entry{parentDirectory};
        return pathEntry.exists() && pathEntry.is_directory();
    }

    void CreateProjectDirectories(const std::filesystem::path& projectDirectory)
    {
        std::filesystem::create_directories(projectDirectory);
        std::filesystem::create_directory(projectDirectory / "assets");
        std::filesystem::create_directory(projectDirectory / "assets" / "audio_clips");
        std::filesystem::create_directory(projectDirectory / "assets" / "concave_colliders");
        std::filesystem::create_directory(projectDirectory / "assets" / "hull_colliders");
        std::filesystem::create_directory(projectDirectory / "assets" / "mesh");
        std::filesystem::create_directory(projectDirectory / "assets" / "shaders");
        std::filesystem::create_directory(projectDirectory / "assets" / "sounds");
        std::filesystem::create_directory(projectDirectory / "assets" / "textures");
        std::filesystem::create_directory(projectDirectory / "config");
        std::filesystem::create_directory(projectDirectory / "scenes");
        std::filesystem::create_directory(projectDirectory / "source");
    }

    void CreateProjectFile(const std::filesystem::path& projectFilePath)
    {
        std::ofstream projectFile{projectFilePath};
        projectFile << "name=" << projectFilePath.stem().string() << '\n'
                    << "initial_scene=" << "SampleScene";
        projectFile.close();
    }
}