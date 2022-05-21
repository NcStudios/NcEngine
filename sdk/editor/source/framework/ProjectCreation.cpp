#include "ProjectCreation.h"
#include "utility/DefaultComponents.h"
#include "config/Config.h"

#include <fstream>

namespace
{
    void CopyFileTemplate(const std::filesystem::path& inFile,
                          const std::filesystem::path& outFile,
                          const std::unordered_map<std::string, std::string>& replace)
    {
        std::ifstream in{inFile};
        if(!in) throw std::runtime_error("Could not open file: " + outFile.string());

        std::ofstream out{outFile};
        if(!out) throw std::runtime_error("Could not open file: " + inFile.string());

        std::string line;
        bool repeatCheck = false;

        while(std::getline(in, line))
        {
            do
            {
                repeatCheck = false;
                if(line.find('@') != line.npos)
                {
                    for(const auto& [oldWord, newWord] : replace)
                    {
                        auto wordPos = line.find(oldWord);
                        if(wordPos == line.npos)
                            continue;

                        line.replace(wordPos, oldWord.size(), newWord);
                        repeatCheck = true; // could be another replacement on this line
                    }
                }
            } while (repeatCheck);

            out << line << '\n';
        }
    }
}

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
        std::filesystem::create_directory(projectDirectory / "assets" / "meshes");
        std::filesystem::create_directory(projectDirectory / "assets" / "shaders");
        std::filesystem::create_directory(projectDirectory / "assets" / "textures");
        std::filesystem::create_directory(projectDirectory / "assets" / "textures" / "cubemaps");
        std::filesystem::create_directory(projectDirectory / "config");
        std::filesystem::create_directory(projectDirectory / "scenes");
        std::filesystem::create_directory(projectDirectory / "source");
    }

    void CreateProjectFile(const std::filesystem::path& projectDirectory, const std::string& projectName)
    {
        const auto replace = std::unordered_map<std::string, std::string>
        {
            {"@PROJECT_NAME@", projectName},
            {"@INITIAL_SCENE@", "LoaderScene"}
        };

        CopyFileTemplate("editor/file_templates/ProjectFile.ncproj", projectDirectory / (projectName + std::string{".ncproj"}), replace);
    }

    void CreateConfig(const std::filesystem::path& projectDirectory, const std::string& projectName)
    {
        const auto directory = projectDirectory.string();
        const auto replace = std::unordered_map<std::string, std::string>
        {
            {"@PROJECT_NAME@", projectName},
            {"@PROJECT_DIRECTORY@", directory}
        };

        CopyFileTemplate("editor/file_templates/config.ini", projectDirectory / "config\\config.ini", replace);
    }

    void CreateMain(const std::filesystem::path& projectDirectory)
    {
        const auto replace = std::unordered_map<std::string, std::string>
        {
            {"@INCLUDE_INITIAL_SCENE@", "scenes\\GeneratedLoadScene.h"},
            {"@INITIAL_SCENE_TYPE@", "GeneratedLoadScene"}
        };

        CopyFileTemplate("editor/file_templates/Main.cpp", projectDirectory / "Main.cpp", replace);
    }

    void CreateCMakeFiles(const std::filesystem::path& projectDirectory)
    {
        std::filesystem::copy_file("editor/file_templates/build.bat", projectDirectory / "build.bat");
        std::filesystem::copy_file("editor/file_templates/RootCMakeLists.txt", projectDirectory / "CMakeLists.txt");
        std::filesystem::copy_file("editor/file_templates/SceneCMakeLists.txt", projectDirectory / "scenes/CMakeLists.txt");
        std::filesystem::copy_file("editor/file_templates/SourceCMakeLists.txt", projectDirectory / "source/CMakeLists.txt");
    }

    void CopyDefaultAssets(const std::filesystem::path& projectDirectory)
    {
        using std::filesystem::copy_file;
        const auto& projectSettings = config::GetProjectSettings();
        auto projectAssetDirectory = projectDirectory / "assets\\";

        {
            copy_file(std::filesystem::path{projectSettings.concaveCollidersPath} / PlaneConcaveColliderPath, projectAssetDirectory / "concave_colliders" / PlaneConcaveColliderPath);
        }
        
        {
            copy_file(std::filesystem::path{projectSettings.hullCollidersPath} / CubeHullColliderPath, projectAssetDirectory / "hull_colliders" / CubeHullColliderPath);
        }

        {
            auto engineMeshDirectory = std::filesystem::path{projectSettings.meshesPath};
            auto projectMeshDirectory = projectAssetDirectory / "meshes/";
            copy_file(engineMeshDirectory / CubeMeshPath, projectMeshDirectory / CubeMeshPath);
            copy_file(engineMeshDirectory / CapsuleMeshPath, projectMeshDirectory / CapsuleMeshPath);
            copy_file(engineMeshDirectory / SphereMeshPath, projectMeshDirectory / SphereMeshPath);
            copy_file(engineMeshDirectory / PlaneMeshPath, projectMeshDirectory / PlaneMeshPath);
        }

        {
            auto engineShaderDirectory = std::filesystem::path{projectSettings.shadersPath};
            auto projectShaderDirectory = projectAssetDirectory / "shaders";
            copy_file(engineShaderDirectory / "EnvironmentFragment.spv", projectShaderDirectory / "EnvironmentFragment.spv");
            copy_file(engineShaderDirectory / "EnvironmentVertex.spv", projectShaderDirectory / "EnvironmentVertex.spv");
            copy_file(engineShaderDirectory / "ParticleFragment.spv", projectShaderDirectory / "ParticleFragment.spv");
            copy_file(engineShaderDirectory / "ParticleVertex.spv", projectShaderDirectory / "ParticleVertex.spv");
            copy_file(engineShaderDirectory / "PbrFragment.spv", projectShaderDirectory / "PbrFragment.spv");
            copy_file(engineShaderDirectory / "PbrVertex.spv", projectShaderDirectory / "PbrVertex.spv");
            copy_file(engineShaderDirectory / "ShadowMappingFragment.spv", projectShaderDirectory / "ShadowMappingFragment.spv");
            copy_file(engineShaderDirectory / "ShadowMappingVertex.spv", projectShaderDirectory / "ShadowMappingVertex.spv");
            copy_file(engineShaderDirectory / "UiFragment.spv", projectShaderDirectory / "UiFragment.spv");
            copy_file(engineShaderDirectory / "UiVertex.spv", projectShaderDirectory / "UiVertex.spv");
            copy_file(engineShaderDirectory / "WireframeFragment.spv", projectShaderDirectory / "WireframeFragment.spv");
            copy_file(engineShaderDirectory / "WireframeVertex.spv", projectShaderDirectory / "WireframeVertex.spv");
        }

        {
            auto engineTextureDirectory = std::filesystem::path{projectSettings.texturesPath};
            auto projectTextureDirectory = projectAssetDirectory / "textures/";
            copy_file(engineTextureDirectory / DefaultBaseColorPath, projectTextureDirectory / DefaultBaseColorPath);
            copy_file(engineTextureDirectory / DefaultNormalPath, projectTextureDirectory / DefaultNormalPath);
            copy_file(engineTextureDirectory / DefaultRoughnessPath, projectTextureDirectory / DefaultRoughnessPath);
        }

        {
            auto engineSkyboxesDirectory = std::filesystem::path{projectSettings.cubeMapsPath};
            auto projectTextureDirectory = projectAssetDirectory / "textures";
            auto projectSkyboxesDirectory = projectTextureDirectory / "cubemaps";
            copy(engineSkyboxesDirectory / "DefaultSkybox", projectSkyboxesDirectory / "DefaultSkybox");
        }
    }
}