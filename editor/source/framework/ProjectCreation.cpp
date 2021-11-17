#include "ProjectCreation.h"
#include "utility/DefaultComponents.h"
#include "config/Config.h"

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
        std::filesystem::create_directory(projectDirectory / "assets" / "meshes");
        std::filesystem::create_directory(projectDirectory / "assets" / "shaders");
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

    void CreateConfig(const std::filesystem::path& projectDirectory, const std::string& projectName)
    {
        std::ofstream file{projectDirectory / "config/config.ini"};
        file << "[project]\n"
             << "project_name=" << projectName << '\n'
             << "log_file_path=" << projectDirectory.string() << '\n'
             << "[memory]\n"
             << "max_dynamic_colliders=25000\n"
             << "max_static_colliders=25000\n"
             << "max_network_dispatchers=0\n"
             << "max_particle_emitters=1000\n"
             << "max_renderers=100000\n"
             << "max_transforms=100000\n"
             << "max_point_lights=10\n"
             << "max_textures=1000\n"
             << "[physics]\n"
             << "fixed_update_interval=0.01667\n"
             << "worldspace_extent=1000\n"
             << "octree_density_threshold=20\n"
             << "octree_minimum_extent=5\n"
             << "[graphics]\n"
             << "use_native_resolution=0\n"
             << "launch_fullscreen=0\n"
             << "screen_width=1000\n"
             << "screen_height=1000\n"
             << "target_fps=60\n"
             << "near_clip=0.5\n"
             << "far_clip=400\n"
             << "shaders_path=" << (projectDirectory / "assets/shaders").string() << '\n'
             << "use_shadows=1";
    }

    void CopyDefaultAssets(const std::filesystem::path& projectDirectory)
    {
        using std::filesystem::copy_file;
        const auto& projectSettings = config::GetProjectSettings();
        auto projectAssetDirectory = projectDirectory / "assets/";

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
            copy_file(engineShaderDirectory / "ParticleFragment.spv", projectShaderDirectory / "ParticleFragment.spv");
            copy_file(engineShaderDirectory / "ParticleVertex.spv", projectShaderDirectory / "ParticleVertex.spv");
            copy_file(engineShaderDirectory / "PhongFragment.spv", projectShaderDirectory / "PhongFragment.spv");
            copy_file(engineShaderDirectory / "PhongVertex.spv", projectShaderDirectory / "PhongVertex.spv");
            copy_file(engineShaderDirectory / "ShadowMappingFragment.spv", projectShaderDirectory / "ShadowMappingFragment.spv");
            copy_file(engineShaderDirectory / "ShadowMappingVertex.spv", projectShaderDirectory / "ShadowMappingVertex.spv");
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
    }
}