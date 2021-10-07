#pragma once

#include "Ecs.h"

#include "Common.h"

namespace nc::editor
{
    /** Loads contents of a scene.gen file into the registry. */
    class SceneReader
    {
        public:
            SceneReader(registry_type* registry, const std::filesystem::path& scenesDirectory, const std::string& sceneName);

        private:
            registry_type* m_registry;
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
    };
}