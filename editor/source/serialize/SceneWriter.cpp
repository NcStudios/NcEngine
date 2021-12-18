#include "SceneWriter.h"
#include "EditorScene.h"
#include "utility/Output.h"

#include <iostream>

namespace
{
    constexpr char TAB[5]{"    "};

    std::string ToString(nc::TechniqueType type)
    {
        if(type == nc::TechniqueType::PhongAndUi)
            return "TechniqueType::PhongAndUi";
        
        return "TechniqueType::None";
    }
}

namespace nc
{
    std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const Vector3& v)
    {
        os << "Vector3{" << v.x << ", " << v.y << ", " << v.z << "}";
        return os;
    }

    std::basic_ostream<char>& operator<<(std::basic_ostream<char>& os, const Quaternion& q)
    {
        os << "Quaternion{" << q.x << ", " << q.y << ", " << q.z << ", " << q.w << "}";
        return os;
    }
}

namespace nc::editor
{
    SceneWriter::SceneWriter(Registry* registry, const std::filesystem::path& scenesDirectory)
        : m_registry{registry},
          m_file{},
          m_handleNames{},
          m_scenesDirectory{scenesDirectory},
          m_sceneName{}
    {
    }

    void SceneWriter::WriteCurrentScene(SceneData* sceneData, const std::string& sceneName)
    {
        m_sceneName = sceneName;

        CreateHandleNames();

        if(!DoFilesExist())
        {
            CreateHeader();
            CreateSource();
        }

        CreateGeneratedSource(sceneData);
    }

    void SceneWriter::WriteNewScene(const std::string& sceneName)
    {
        m_sceneName = sceneName;

        if(DoFilesExist())
        {
            Output::LogError("Failure writing scene: Scene already exists with name: " + sceneName);
        }

        CreateHeader();
        CreateSource();

        auto filePath = m_scenesDirectory / (m_sceneName + FileExtension::Generated.data());
        m_file.open(filePath);
        m_file << "/** Generated source code for scene: " << m_sceneName << " - DO NOT MODIFY */\n";
        m_file.close();
    }

    void SceneWriter::CreateHandleNames()
    {
        const std::string entityString{"entity"};
        const std::string underscore{"_"};
        size_t i = 0u;
        for(auto entity : m_registry->ViewAll<Entity>())
        {
            ++i;
            auto* tag = m_registry->Get<Tag>(entity);

            m_handleNames.emplace
            (
                entity.Index(),
                entityString + std::to_string(i) + underscore + StripCharacter(std::string{tag->Value()}, ' ')
            );
        }
    }

    bool SceneWriter::DoFilesExist()
    {
        auto headerEntry = std::filesystem::directory_entry{m_scenesDirectory.string() + m_sceneName + FileExtension::Header.data()};
        return headerEntry.exists();
    }

    void SceneWriter::CreateHeader()
    {
        auto filePath = m_scenesDirectory / (m_sceneName + FileExtension::Header.data());
        std::ofstream file{filePath};
        file << "#pragma once\n\n"
             << "#include \"NcEngine.h\"\n\n"
             << "namespace project\n"
             << "{\n"
             << TAB << "class " << m_sceneName << " : public nc::Scene\n"
             << TAB << "{\n"
             << TAB << TAB << "public:\n"
             << TAB << TAB << TAB << "void Load(nc::NcEngine* engine) override;\n"
             << TAB << TAB << TAB << "void Unload() override;\n"
             << TAB << "};\n"
             << "}";
    }

    void SceneWriter::CreateSource()
    {
        auto filePath = m_scenesDirectory / (m_sceneName + FileExtension::Source.data());
        std::ofstream file{filePath};
        file << "#include \"" << m_sceneName << FileExtension::Header << "\"\n"
             << "#include \"" << m_sceneName << FileExtension::Generated << "\"\n\n"
             << "namespace project\n"
             << "{\n"
             << TAB << "void " << m_sceneName << "::Load(nc::NcEngine* engine)\n"
             << TAB << "{\n"
             << TAB << TAB << "nc::generated::" << m_sceneName << "::Init(engine);\n"
             << TAB << "}\n\n"
             << TAB << "void " << m_sceneName << "::Unload()\n"
             << TAB << "{\n"
             << TAB << "}\n"
             << "}";
    }

    void SceneWriter::CreateGeneratedSource(SceneData* sceneData)
    {
        auto filePath = m_scenesDirectory / (m_sceneName + FileExtension::Source.data());
        m_file.open(filePath);
        if(!m_file.is_open())
        {
            Output::LogError("Failure opening scene:", m_scenesDirectory.string() + m_sceneName);
        }

        m_file << "/** Generated source code for scene: " << m_sceneName << " - DO NOT MODIFY */\n"
               << "#pragma once\n\n"
               << "#include \"NcEngine.h\"\n\n"
               << "namespace nc::generated::" << m_sceneName << '\n'
               << "{\n"
               << "using namespace nc;\n"
               << "void Init(NcEngine* engine)\n"
               << "{\n"
               << "auto* registry = engine->Registry();\n";

        for(auto e : m_registry->ViewAll<Entity>())
        {
            WriteEntity(e);
        }

        WriteSceneData(sceneData);

        m_file << "}\n"
               << "}";

        m_file.close();
    }

    void SceneWriter::WriteSceneData(SceneData* sceneData)
    {
        if(sceneData->mainCamera.Valid() && m_registry->Contains<Camera>(sceneData->mainCamera))
        {
            m_file << SceneMacro::SetCamera << "( " << m_handleNames.at(sceneData->mainCamera.Index()) << " );\n";
        }

        if(sceneData->audioListener.Valid())
        {
            m_file << SceneMacro::RegisterAudioListener << "( " << m_handleNames.at(sceneData->audioListener.Index()) << " );\n";
        }
    }

    void SceneWriter::WriteEntity(Entity entity)
    {
        auto tag = m_registry->Get<Tag>(entity)->Value();

        if(tag == EditorScene::EditorCameraTag)
            return;

        const auto& handleName = m_handleNames.at(entity.Index());
        auto* transform = m_registry->Get<Transform>(entity);
        auto parent = transform->Parent();
        auto parentHandle = parent.Valid() ? m_handleNames.at(parent.Index()) : std::string{"Entity::Null()"};

        m_file << SceneMacro::AddEntity << "("
               << handleName << " , "
               << SceneMacro::EntityInfo << "( "
               << transform->LocalPosition() << " , "
               << transform->LocalRotation() << " , "
               << transform->LocalScale() << " , "
               << parentHandle << " , "
               << "\"" << tag << "\" , "
               << static_cast<unsigned>(entity.Layer()) << " , "
               << static_cast<unsigned>(entity.Flags()) << " ) );\n";

        WriteCamera(entity, handleName);
        WriteCollider(entity, handleName);
        WriteConcaveCollider(entity, handleName);
        WritePhysicsBody(entity, handleName);
        WritePointLight(entity, handleName);
        WriteMeshRenderer(entity, handleName);

        m_file << '\n';
    }

    void SceneWriter::WriteCollider(Entity entity, const std::string& handleName)
    {
        auto* collider = m_registry->Get<Collider>(entity);

        if(!collider)
            return;

        const auto& center = collider->GetInfo().offset;
        const auto& scale = collider->GetInfo().scale;
        auto isTrigger = collider->IsTrigger();

        switch(collider->GetType())
        {
            case ColliderType::Box:
            {
                m_file << SceneMacro::AddBoxCollider << "( " << handleName << " , " << SceneMacro::BoxProperties << "( " << center << " , " << scale << " ) , " << isTrigger << " )\n";
                break;
            }
            case ColliderType::Capsule:
            {
                auto height = scale.y * 2.0f;
                auto radius = scale.x * 0.5f;
                m_file << SceneMacro::AddCapsuleCollider << "( " << handleName << " , " << SceneMacro::CapsuleProperties << "( " << center << " , " << height << " , " << radius << " ) , " << isTrigger << " )\n";
                break;
            }
            case ColliderType::Hull:
            {
                m_file << SceneMacro::AddHullCollider << "( " << handleName << " , " << SceneMacro::HullProperties << "( " << "\"" << collider->GetInfo().hullAssetPath << "\" ) , " << isTrigger << " )\n";
                break;
            }
            case ColliderType::Sphere:
            {
                auto radius = scale.x * 0.5f;
                m_file << SceneMacro::AddSphereCollider << "( " << handleName << " , " << SceneMacro::SphereProperties << "( " << center << " , " << radius << " ) , " << isTrigger << " );\n";
                break;
            }
        }
    }

    void SceneWriter::WriteConcaveCollider(Entity entity, const std::string& handleName)
    {
        auto* collider = m_registry->Get<ConcaveCollider>(entity);

        if(!collider)
            return;

        m_file << SceneMacro::AddConcaveCollider << "( " << handleName << " , \"" << collider->GetPath() << "\" )\n";

    }

    void SceneWriter::WritePhysicsBody(Entity entity, const std::string& handleName)
    {
        /** @todo need linear and angular freedoms*/

        auto* body = m_registry->Get<PhysicsBody>(entity);

        if(!body)
            return;

        m_file << SceneMacro::AddPhysicsBody << "( "
               << handleName << " , "
               << SceneMacro::PhysicsProperties << "( "
               << 1.0f / body->GetInverseMass() << " , "
               << body->GetDrag() << " , "
               << body->GetAngularDrag() << " , "
               << body->GetRestitution() << " , "
               << body->GetFriction() << " , "
               << body->UseGravity() << " , "
               << body->IsKinematic() << " ) );\n";
    }

    void SceneWriter::WritePointLight(Entity entity, const std::string& handleName)
    {
        auto* light = m_registry->Get<PointLight>(entity);

        if(!light)
            return;

        const auto& info = light->GetInfo();

        m_file << SceneMacro::AddPointLight << "( "
               << handleName << " , "
               << SceneMacro::PointLightInfo << "( "
               << "DirectX::XMMATRIX{} , "
               << info.pos << " , "
               << "1 , "
               << info.ambient << " , "
               << "0.0f , "
               << info.diffuseColor << " , "
               << "0.0f , "
               << info.diffuseIntensity << " ) );\n";
    }

    void SceneWriter::WriteMeshRenderer(Entity entity, const std::string& handleName)
    {
        auto* renderer = m_registry->Get<MeshRenderer>(entity);

        if(!renderer)
            return;

        const auto& material = renderer->GetMaterial();
        auto techniqueType = ToString(renderer->GetTechniqueType());

        m_file << SceneMacro::AddMeshRenderer << "( "
               << handleName << " , "
               << SceneMacro::Material << "( "
               << "\"" << renderer->GetMeshPath() << "\" , "
               << "\"" << material.baseColor << "\" , "
               << "\"" << material.normal << "\" , "
               << "\"" << material.roughness << "\" , "
               << "\"" << material.metallic << "\" ) , "
               << techniqueType << " );\n";
    }

    void SceneWriter::WriteCamera(Entity entity, const std::string& handleName)
    {
        auto* camera = m_registry->Get<Camera>(entity);

        if(!camera)
            return;

        m_file << SceneMacro::AddCamera << "( " << handleName << " );\n";
    }
}