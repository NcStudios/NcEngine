#pragma once

#include "Ecs.h"

#include "Common.h"

namespace nc::editor
{
    /** Writes the registry's contents to a scene.gen file. WriteNewScene will
     *  create the header, source, and generated files for a new scene. */
    class SceneWriter
    {
        public:
            SceneWriter(registry_type* registry, const std::filesystem::path& scenesDirectory);
            void WriteCurrentScene(const std::string& sceneName);
            void WriteNewScene(const std::string& sceneName);

        private:
            registry_type* m_registry;
            std::ofstream m_file;
            std::unordered_map<Entity::index_type, std::string> m_handleNames;
            std::filesystem::path m_scenesDirectory;
            std::string m_sceneName;

            void CreateHandleNames();
            bool DoFilesExist();
            void CreateHeader();
            void CreateSource();
            void CreateGeneratedSource();
            void WriteEntity(Entity entity);
            void WriteCollider(Entity entity, const std::string& handleName);
            void WriteConcaveCollider(Entity entity, const std::string& handleName);
            void WritePhysicsBody(Entity entity, const std::string& handleName);
            void WritePointLight(Entity entity, const std::string& handleName);
            void WriteMeshRenderer(Entity entity, const std::string& handleName);
    };
}