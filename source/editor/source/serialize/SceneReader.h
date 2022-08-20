#pragma once

#include "ecs/Registry.h"

#include "Common.h"

namespace nc::graphics
{
struct NcGraphics;
}

namespace nc::editor
{
    /** Loads contents of a scene.gen file into the registry. */
    class SceneReader
    {
        public:
            SceneReader(Registry* registry, graphics::NcGraphics* graphics, const std::filesystem::path& scenesDirectory, const std::string& sceneName);

        private:
            Registry* m_registry;
            graphics::NcGraphics* m_graphics;
            std::ifstream m_file;
            std::unordered_map<std::string, Entity> m_handleNames;
            std::filesystem::path m_scenesDirectory;
            std::string m_sceneName;

            Entity DispatchAction(Entity entity, const std::string& actionDescription, std::stringstream& args);
            Entity LoadEntity(std::stringstream& args);
            void LoadBoxCollider(Entity entity, std::stringstream& args);
            void LoadCapsuleCollider(Entity entity, std::stringstream& args);
            void LoadHullCollider(Entity entity, std::stringstream& args);
            void LoadSphereCollider(Entity entity, std::stringstream& args);
            void LoadConcaveCollider(Entity entity, std::stringstream& args);
            void LoadPhysicsBody(Entity entity, std::stringstream& args);
            void LoadPointLight(Entity entity, std::stringstream& args);
            void LoadMeshRenderer(Entity entity, std::stringstream& args);
            void LoadCamera(Entity entity, std::stringstream& args);
            void LoadSkybox(std::stringstream& args);
    };
}