#include "SceneReader.h"
#include "graphics/GraphicsModule.h"
#include "utility/DefaultComponents.h"
#include "utility/Output.h"

#include <iostream>
#include <sstream>

namespace
{
    using namespace nc;

    nc::graphics::TechniqueType ToTechniqueType(const std::string& name)
    {
        if(name == "TechniqueType::PhongAndUi")
            return nc::graphics::TechniqueType::PhongAndUi;

        return nc::graphics::TechniqueType::None;
    }

    std::string SplitLineToActionDescriptionAndArgs(std::string line, std::stringstream& argsOut)
    {
        line = editor::StripCharacter(std::move(line), ',');
        auto argsStart = line.find_first_of('(');
        auto argsEnd = line.find_last_of(')');
        auto description = line.substr(0u, argsStart);
        argsOut.str(std::string{});
        argsOut.clear();
        argsOut << line.substr(argsStart + 1, argsEnd - (argsStart + 1));
        return description;
    }

    std::string ReadTokenFromAction(std::stringstream& actionArgs)
    {
        std::string out;
        actionArgs >> out;
        return out;
    }

    float ReadFloatFromAction(std::stringstream& actionArgs)
    {
        float out;
        actionArgs >> out;
        return out;
    }

    bool ReadBoolFromAction(std::stringstream& actionArgs)
    {
        bool out;
        actionArgs >> out;
        return out;
    }

    Vector3 ReadVector3FromAction(std::stringstream& actionArgs)
    {
        Vector3 out;
        actionArgs.ignore(std::numeric_limits<std::streamsize>::max(), '{');
        actionArgs >> out.x >> out.y >> out.z;
        actionArgs.ignore(std::numeric_limits<std::streamsize>::max(), '}');
        return out;
    }

    Quaternion ReadQuaternionFromAction(std::stringstream& actionArgs)
    {
        Quaternion out;
        actionArgs.ignore(std::numeric_limits<std::streamsize>::max(), '{');
        actionArgs >> out.x >> out.y >> out.z >> out.w;
        actionArgs.ignore(std::numeric_limits<std::streamsize>::max(), '}');
        return out;
    }

    std::string ReadQuotedStringFromAction(std::stringstream& actionArgs)
    {
        std::string out;
        actionArgs.ignore(std::numeric_limits<std::streamsize>::max(), '\"');
        std::getline(actionArgs, out, '\"');
        return out;
    }

    Entity::layer_type ReadLayerFromAction(std::stringstream& actionArgs)
    {
        unsigned out;
        actionArgs >> out;
        return static_cast<Entity::layer_type>(out);
    }

    Entity::flags_type ReadEntityFlagsFromAction(std::stringstream& actionArgs)
    {
        unsigned out;
        actionArgs >> out;
        return static_cast<Entity::layer_type>(out);
    }

    Entity ReadParentHandleFromAction(std::stringstream& actionArgs, const std::unordered_map<std::string, Entity>& entityHandles)
    {
        std::string handle;
        actionArgs >> handle;
        return handle == "Entity::Null()" ? Entity::Null() : entityHandles.at(handle);
    }
}

namespace nc::editor
{
    SceneReader::SceneReader(Registry* registry, graphics::GraphicsModule* graphics, const std::filesystem::path& scenesDirectory, const std::string& sceneName)
        : m_registry{registry},
          m_graphics{graphics},
          m_file{scenesDirectory / (sceneName + FileExtension::Generated.data())},
          m_handleNames{},
          m_scenesDirectory{scenesDirectory},
          m_sceneName{sceneName}
    {
        if(!m_file.is_open())
        {
            Output::LogError("Failure opening scene:", m_scenesDirectory.string() + m_sceneName + FileExtension::Generated.data());
            return;
        }

        Entity currentEntity;
        std::string line;
        std::stringstream args;

        while(!m_file.eof())
        {
            if(m_file.fail())
            {
                Output::LogError("Failure reading scene:", m_scenesDirectory.string() + m_sceneName + FileExtension::Generated.data());
                m_file.close();
                return;
            }

            std::getline(m_file, line, '\n');

            if(line.starts_with("NC_EDITOR"))
            {
                auto actionDescription = SplitLineToActionDescriptionAndArgs(line, args);
                currentEntity = DispatchAction(currentEntity, actionDescription, args);
            }
        }

        m_file.close();
    }
    
    Entity SceneReader::DispatchAction(Entity currentEntity, const std::string& actionDescription, std::stringstream& args)
    {
             if(actionDescription == SceneMacro::AddEntity)          return LoadEntity(args);
        else if(actionDescription == SceneMacro::AddMeshRenderer)    LoadMeshRenderer(currentEntity, args);
        else if(actionDescription == SceneMacro::AddPhysicsBody)     LoadPhysicsBody(currentEntity, args);
        else if(actionDescription == SceneMacro::AddBoxCollider)     LoadBoxCollider(currentEntity, args);
        else if(actionDescription == SceneMacro::AddCapsuleCollider) LoadCapsuleCollider(currentEntity, args);
        else if(actionDescription == SceneMacro::AddHullCollider)    LoadHullCollider(currentEntity, args);
        else if(actionDescription == SceneMacro::AddSphereCollider)  LoadSphereCollider(currentEntity, args);
        else if(actionDescription == SceneMacro::AddConcaveCollider) LoadConcaveCollider(currentEntity, args);
        else if(actionDescription == SceneMacro::AddPointLight)      LoadPointLight(currentEntity, args);
        else if(actionDescription == SceneMacro::AddCamera)          LoadCamera(currentEntity, args);
        else if(actionDescription == SceneMacro::SetSkybox)          LoadSkybox(args);

        return currentEntity;
    }

    Entity SceneReader::LoadEntity(std::stringstream& args)
    {
        auto handleName = ReadTokenFromAction(args);
        ReadTokenFromAction(args);
        auto position = ReadVector3FromAction(args);
        auto rotation = ReadQuaternionFromAction(args);
        auto scale = ReadVector3FromAction(args);
        auto parent = ReadParentHandleFromAction(args, m_handleNames);
        auto tag = ReadQuotedStringFromAction(args);
        auto layer = ReadLayerFromAction(args);
        auto flags = ReadEntityFlagsFromAction(args);

        EntityInfo info
        {
            .position = position,
            .rotation = rotation,
            .scale = scale,
            .parent = parent,
            .tag = std::move(tag),
            .layer = layer,
            .flags = flags
        };

        Entity entity = m_registry->Add<Entity>(std::move(info));
        m_handleNames.emplace(std::move(handleName), entity);
        return entity;
    }

    void SceneReader::LoadBoxCollider(Entity entity, std::stringstream& args)
    {
        ReadTokenFromAction(args);
        ReadTokenFromAction(args);
        physics::BoxProperties properties{ .center = ReadVector3FromAction(args), .extents = ReadVector3FromAction(args) };
        m_registry->Add<physics::Collider>(entity, properties, ReadBoolFromAction(args));
    }

    void SceneReader::LoadCapsuleCollider(Entity entity, std::stringstream& args)
    {
        ReadTokenFromAction(args);
        ReadTokenFromAction(args);
        physics::CapsuleProperties properties
        {
            .center = ReadVector3FromAction(args),
            .height = ReadFloatFromAction(args),
            .radius = ReadFloatFromAction(args)
        };
        m_registry->Add<physics::Collider>(entity, properties, ReadBoolFromAction(args));
    }

    void SceneReader::LoadHullCollider(Entity entity, std::stringstream& args)
    {
        ReadTokenFromAction(args);
        ReadTokenFromAction(args);
        physics::HullProperties properties{ .assetPath = ReadQuotedStringFromAction(args) };
        bool isTrigger = ReadBoolFromAction(args);

        /** @todo should have specific exception so we don't mask other failures*/
        try
        {
            m_registry->Add<physics::Collider>(entity, properties, isTrigger);
        }
        catch(const std::runtime_error&)
        {
            Output::LogError("Failure adding required hull collider:", properties.assetPath);
            AddDefaultHullCollider(m_registry, entity, isTrigger);
        }
    }

    void SceneReader::LoadSphereCollider(Entity entity, std::stringstream& args)
    {
        ReadTokenFromAction(args);
        ReadTokenFromAction(args);
        physics::SphereProperties properties{ .center = ReadVector3FromAction(args), .radius = ReadFloatFromAction(args) };
        m_registry->Add<physics::Collider>(entity, properties, ReadBoolFromAction(args));
    }

    void SceneReader::LoadConcaveCollider(Entity entity, std::stringstream& args)
    {
        ReadTokenFromAction(args);
        auto assetPath = ReadQuotedStringFromAction(args);

        /** @todo should have specific exception so we don't mask other failures*/
        try
        {
            m_registry->Add<physics::ConcaveCollider>(entity, assetPath);
        }
        catch(const std::runtime_error&)
        {
            Output::LogError("Failure adding required concave collider:", assetPath);
            AddDefaultConcaveCollider(m_registry, entity);
        }
    }

    void SceneReader::LoadPhysicsBody(Entity entity, std::stringstream& args)
    {
        /** @todo need lin/ang freedom */

        ReadTokenFromAction(args);
        ReadTokenFromAction(args);
        physics::PhysicsProperties properties
        {
            .mass = ReadFloatFromAction(args),
            .drag = ReadFloatFromAction(args),
            .angularDrag = ReadFloatFromAction(args),
            .restitution = ReadFloatFromAction(args),
            .friction = ReadFloatFromAction(args),
            .useGravity = ReadBoolFromAction(args),
            .isKinematic = ReadBoolFromAction(args)
        };
        m_registry->Add<physics::PhysicsBody>(entity, properties);
    }

    void SceneReader::LoadPointLight(Entity entity, std::stringstream& args)
    {
        ReadTokenFromAction(args);
        ReadTokenFromAction(args);
        ReadTokenFromAction(args);
        auto pos = ReadVector3FromAction(args);
        ReadTokenFromAction(args);
        auto ambient = ReadVector3FromAction(args);
        ReadFloatFromAction(args);
        auto diffuseColor = ReadVector3FromAction(args);
        auto diffuseIntensity = ReadFloatFromAction(args);

        graphics::PointLightInfo properties
        {
            .pos = pos,
            .ambient = ambient,
            .diffuseColor = diffuseColor,
            .diffuseIntensity = diffuseIntensity,
        };
        m_registry->Add<graphics::PointLight>(entity, properties);
    }

    void SceneReader::LoadMeshRenderer(Entity entity, std::stringstream& args)
    {
        ReadTokenFromAction(args);
        ReadTokenFromAction(args);
        std::string mesh = ReadQuotedStringFromAction(args);
        graphics::Material material
        {
            .baseColor = ReadQuotedStringFromAction(args),
            .normal = ReadQuotedStringFromAction(args),
            .roughness = ReadQuotedStringFromAction(args),
            .metallic = ReadQuotedStringFromAction(args)
        };
        auto technique = ToTechniqueType(ReadTokenFromAction(args));

        /** @todo should have specific exception so we don't mask other failures*/
        try
        {
            m_registry->Add<graphics::MeshRenderer>(entity, std::move(mesh), std::move(material), technique);
        }
        catch(const std::exception&)
        {
            Output::LogError("Failure adding required mesh assets");
            AddDefaultMeshRenderer(m_registry, entity);
        }
    }

    void SceneReader::LoadCamera(Entity entity, std::stringstream& args)
    {
        ReadTokenFromAction(args);
        m_registry->Add<graphics::Camera>(entity);
    }

    void SceneReader::LoadSkybox(std::stringstream& args)
    {
        const auto path = ReadQuotedStringFromAction(args);
        if (path.empty())
        {
            m_graphics->ClearEnvironment();
        }

        if (path == DefaultSkyboxPath)
        {
            m_graphics->SetSkybox(DefaultSkyboxPath);
        }

        m_graphics->SetSkybox(path);
    }
}