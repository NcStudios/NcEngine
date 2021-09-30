#include "SceneWriter.h"
#include "utility/Output.h"

#include <iostream>

namespace
{
    constexpr char TAB[5]{"    "};

    std::string ToString(nc::graphics::TechniqueType type)
    {
        if(type == nc::graphics::TechniqueType::PhongAndUi)
            return "graphics::TechniqueType::PhongAndUi";
        
        return "graphics::TechniqueType::None";
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
    SceneWriter::SceneWriter(registry_type* registry, const std::filesystem::path& scenesDirectory)
        : m_registry{registry},
          m_file{},
          m_handleNames{},
          m_scenesDirectory{scenesDirectory},
          m_sceneName{}
    {
    }

    void SceneWriter::WriteCurrentScene(const std::string& sceneName)
    {
        m_sceneName = sceneName;

        CreateHandleNames();

        if(!DoFilesExist())
        {
            CreateHeader();
            CreateSource();
        }
        
        CreateGeneratedSource();
    }

    void SceneWriter::WriteNewScene(const std::string& sceneName)
    {
        m_sceneName = sceneName;

        if(DoFilesExist())
        {
            Output::Log("SceneWriter::WriteNewScene - Scene already exists with name: " + sceneName);
        }
        
        CreateHeader();
        CreateSource();
        
        auto filePath = m_scenesDirectory / (m_sceneName + GeneratedSourceExtension);
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
        auto headerEntry = std::filesystem::directory_entry{m_scenesDirectory.string() + m_sceneName + HeaderExtension};
        return headerEntry.exists();
    }

    void SceneWriter::CreateHeader()
    {
        auto filePath = m_scenesDirectory / (m_sceneName + HeaderExtension);
        std::ofstream file{filePath};
        file << "#pragma once\n\n"
             << "#include \"Ecs.h\"\n"
             << "#include \"scene/Scene.h\"\n\n"
             << "namespace project\n"
             << "{\n"
             << TAB << "class " << m_sceneName << " : public nc::scene::Scene\n"
             << TAB << "{\n"
             << TAB << TAB << "public:\n"
             << TAB << TAB << TAB << "void Load(nc::registry_type* registry) override;\n"
             << TAB << TAB << TAB << "void Unload() override;\n"
             << TAB << "}\n"
             << "}";
    }

    void SceneWriter::CreateSource()
    {
        auto filePath = m_scenesDirectory / (m_sceneName + SourceExtension);
        std::ofstream file{filePath};
        file << "#include \"" << m_sceneName << HeaderExtension << "\"\n"
             << "#include \"" << m_sceneName << GeneratedSourceExtension << "\"\n\n"
             << "namespace project\n"
             << "{\n"
             << TAB << "void " << m_sceneName << "::Load(nc::registry_type* registry)\n"
             << TAB << "{\n"
             << TAB << TAB << "nc::generated::" << m_sceneName << "::Init(registry);\n"
             << TAB << "}\n\n"
             << TAB << "void " << m_sceneName << "::Unload()\n"
             << TAB << "{\n"
             << TAB << "}\n"
             << "}";
    }

    void SceneWriter::CreateGeneratedSource()
    {
        auto filePath = m_scenesDirectory / (m_sceneName + GeneratedSourceExtension);
        m_file.open(filePath);
        if(!m_file.is_open())
        {
            Output::Log("SceneSerializer - Failure opening file");
            Output::Log(m_scenesDirectory.string() + m_sceneName);
        }

        m_file << "/** Generated source code for scene: " << m_sceneName << " - DO NOT MODIFY */\n"
               << "#pragma once\n\n"
               << "#include \"Ecs.h\"\n\n"
               << "namespace nc::generated::" << m_sceneName << '\n'
               << "{\n"
               << "using namespace nc;\n"
               << "void Init(registry_type* registry)\n"
               << "{\n";

        for(auto e : m_registry->ViewAll<Entity>())
        {
            WriteEntity(e);
        }

        m_file << "}\n"
               << "}";

        m_file.close();
    }

    void SceneWriter::WriteEntity(Entity entity)
    {
        auto tag = m_registry->Get<Tag>(entity)->Value();
        const auto& handleName = m_handleNames.at(entity.Index());
        auto* transform = m_registry->Get<Transform>(entity);
        auto pos = transform->GetLocalPosition();
        auto rot = transform->GetLocalRotation();
        auto scl = transform->GetLocalScale();
        auto parent = transform->GetParent();
        auto parentHandle = parent.Valid() ? m_handleNames.at(entity.Index()) : std::string{"Entity::Null()"};

        m_file << "NC_SCENE_ACTION_ADD_ENTITY( "
               << handleName << " , "
               << pos << " , "
               << rot << " , "
               << scl << " , "
               << parentHandle << " , "
               << "\"" << tag << "\" , "
               << static_cast<unsigned>(entity.Layer()) << " , "
               << static_cast<unsigned>(entity.Flags()) << " )\n";

        WriteCollider(entity, handleName);
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
                m_file << "NC_SCENE_ACTION_ADD_BOX_COLLIDER( " 
                       << handleName << " , "
                       << center << " , "
                       << scale << " < "
                       << isTrigger << " )\n";
                break;
            }
            case ColliderType::Capsule:
            {
                auto height = scale.y * 2.0f;
                auto radius = scale.x * 0.5f;
                m_file << "NC_SCENE_ACTION_ADD_CAPSULE_COLLIDER( "
                       << handleName << " , "
                       << center << " , "
                       << height << " , "
                       << radius << " , "
                       << isTrigger << " )\n";
                break;
            }
            case ColliderType::Hull:
            {
                m_file << "NC_SCENE_ACTION_ADD_HULL_COLLIDER( "
                       << handleName << " , "
                       << "\"" << collider->GetInfo().hullAssetPath << "\" , "
                       << isTrigger << " )\n";
                break;
            }
            case ColliderType::Sphere:
            {
                auto radius = scale.x * 0.5f;
                m_file << "NC_SCENE_ACTION_ADD_SPHERE_COLLIDER( "
                       << handleName << " , "
                       << center << " , "
                       << radius << " , "
                       << isTrigger << " )\n";
                break;
            }
        }
    }
    
    void SceneWriter::WritePhysicsBody(Entity entity, const std::string& handleName)
    {
        /** @todo need linear and angular freedoms*/

        auto* body = m_registry->Get<PhysicsBody>(entity);
        
        if(!body)
            return;

        m_file << "NC_SCENE_ACTION_ADD_PHYSICS_BODY( "
               << handleName << " , "
               << 1.0f / body->GetInverseMass() << " , "
               << body->GetDrag() << " , "
               << body->GetAngularDrag() << " , "
               << body->GetRestitution() << " , "
               << body->GetFriction() << " , "
               << body->UseGravity() << " , "
               << body->IsKinematic() << " )\n";
    }

    void SceneWriter::WritePointLight(Entity entity, const std::string& handleName)
    {
        auto* light = m_registry->Get<PointLight>(entity);

        if(!light)
            return;
        
        const auto& info = light->GetInfo();

        m_file << "NC_SCENE_ACTION_ADD_POINT_LIGHT( "
               << handleName << " , "
               << info.pos << " , "
               << info.ambient << " , "
               << info.diffuseColor << " , "
               << info.diffuseIntensity << " , "
               << info.attConst << " , "
               << info.attLin << " , "
               << info.attQuad << " )\n";
    }

    void SceneWriter::WriteMeshRenderer(Entity entity, const std::string& handleName)
    {
        auto* renderer = m_registry->Get<MeshRenderer>(entity);

        if(!renderer)
            return;
        
        const auto& material = renderer->GetMaterial();
        auto techniqueType = ToString(renderer->GetTechniqueType());

        m_file << "NC_SCENE_ACTION_ADD_MESH_RENDERER( "
               << handleName << " , "
               << "\"" << renderer->GetMeshPath() << "\" , "
               << "\"" << material.baseColor << "\" , "
               << "\"" << material.normal << "\" , "
               << "\"" << material.roughness << "\" , "
               << techniqueType << " )\n";
    }
}