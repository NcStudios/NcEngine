#pragma once

#include "ecs/Registry.h"

#include "Common.h"
#include "framework/SceneData.h"

namespace nc::editor
{
    /** Writes the registry's contents to a scene.inl file. WriteNewScene will
     *  create the header, source, and generated files for a new scene. */
    class SceneWriter
    {
        public:
            SceneWriter(Registry* registry, const std::filesystem::path& scenesDirectory);
            void WriteCurrentScene(SceneData* sceneData, const std::string& sceneName);
            void WriteNewScene(const std::string& sceneName);

        private:
            Registry* m_registry;
            std::ofstream m_file;
            std::unordered_map<Entity::index_type, std::string> m_handleNames;
            std::filesystem::path m_scenesDirectory;
            std::string m_sceneName;

            void CreateHandleNames();
            bool DoFilesExist();
            void CreateHeader();
            void CreateSource();
            void CreateGeneratedSource(SceneData* sceneData);
            void WriteSceneData(SceneData* sceneData);
            void WriteEntity(Entity entity);
            void WriteCollider(Entity entity, const std::string& handleName);
            void WriteConcaveCollider(Entity entity, const std::string& handleName);
            void WritePhysicsBody(Entity entity, const std::string& handleName);
            void WritePointLight(Entity entity, const std::string& handleName);
            void WriteMeshRenderer(Entity entity, const std::string& handleName);
            void WriteCamera(Entity entity, const std::string& handleName);
    };
}