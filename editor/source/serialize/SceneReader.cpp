#include "SceneReader.h"
#include "utility/Output.h"

#include <iostream>
#include <sstream>

#define PRINT_SERIALIZATION_PROCESS 0

namespace
{
    nc::graphics::TechniqueType ToTechniqueType(const std::string& name)
    {
        if(name == "graphics::TechniqueType::PhongAndUi")
            return nc::graphics::TechniqueType::PhongAndUi;
        
        return nc::graphics::TechniqueType::None;
    }
}

namespace nc::editor
{
    SceneReader::SceneReader(registry_type* registry, const std::filesystem::path& scenesDirectory, const std::string& sceneName)
        : m_registry{registry},
          m_file{scenesDirectory / (sceneName + GeneratedSourceExtension)},
          m_handleNames{},
          m_scenesDirectory{scenesDirectory},
          m_sceneName{sceneName}
    {
        if(!m_file.is_open())
        {
            Output::Log("SceneSerializer::Load - Failure opening file");
            Output::Log(m_scenesDirectory.string() + m_sceneName + GeneratedSourceExtension);
            return;
        }

        Entity currentEntity;
        std::string line;
        std::stringstream args;

        while(!m_file.eof())
        {
            if(m_file.fail())
                Output::Log("SceneSerializer::Load - Failure reading scene");

            std::getline(m_file, line, '\n');

            if(line.starts_with("NC_SCENE_ACTION"))
            {
                line = StripCharacter(std::move(line), ',');

                #if PRINT_SERIALIZATION_PROCESS
                std::cerr << "Found Action: " << line << '\n';
                #endif

                auto argsStart = line.find_first_of('(');
                auto argsEnd = line.find_last_of(')');
                auto actionDescription = line.substr(0u, argsStart);
                args.str(std::string{});
                args.clear();
                args << line.substr(argsStart + 1, argsEnd - (argsStart + 1));

                currentEntity = DispatchAction(currentEntity, actionDescription, args);
            }
        }

        m_file.close();
    }
    
    Entity SceneReader::DispatchAction(Entity currentEntity, const std::string& actionDescription, std::stringstream& args)
    {
        if(actionDescription == "NC_SCENE_ACTION_ADD_ENTITY")
        {
            return LoadEntity(args);
        }
        else if(actionDescription == "NC_SCENE_ACTION_ADD_BOX_COLLIDER")
        {
            LoadBoxCollider(currentEntity, args);
        }
        else if(actionDescription == "NC_SCENE_ACTION_ADD_CAPSULE_COLLIDER")
        {
            LoadCapsuleCollider(currentEntity, args);
        }
        else if(actionDescription == "NC_SCENE_ACTION_ADD_HULL_COLLIDER")
        {
            LoadHullCollider(currentEntity, args);
        }
        else if(actionDescription == "NC_SCENE_ACTION_ADD_SPHERE_COLLIDER")
        {
            LoadSphereCollider(currentEntity, args);
        }
        else if(actionDescription == "NC_SCENE_ACTION_ADD_PHYSICS_BODY")
        {
            LoadPhysicsBody(currentEntity, args);
        }
        else if(actionDescription == "NC_SCENE_ACTION_ADD_POINT_LIGHT")
        {
            LoadPointLight(currentEntity, args);
        }
        else if(actionDescription == "NC_SCENE_ACTION_ADD_MESH_RENDERER")
        {
            LoadMeshRenderer(currentEntity, args);
        }

        return currentEntity;
    }

    Entity SceneReader::LoadEntity(std::stringstream& args)
    {
        EntityInfo info;
        std::string handleName, parentHandleName;
        unsigned layer, flags;

        args >> handleName;
        args.ignore(std::numeric_limits<std::streamsize>::max(), '{');
        args >> info.position.x >> info.position.y >> info.position.z;
        args.ignore(std::numeric_limits<std::streamsize>::max(), '{');
        args >> info.rotation.x >> info.rotation.y >> info.rotation.z >> info.rotation.w;
        args.ignore(std::numeric_limits<std::streamsize>::max(), '{');
        args >> info.scale.x >> info.scale.y >> info.scale.z;
        args.ignore(std::numeric_limits<std::streamsize>::max(), '}');
        args >> parentHandleName;
        args.ignore(std::numeric_limits<std::streamsize>::max(), '\"');
        std::getline(args, info.tag, '\"');
        args >> layer;
        args >> flags;

        info.layer = static_cast<EntityTraits::layer_type>(layer);
        info.flags = static_cast<EntityTraits::flags_type>(flags);

        if(parentHandleName == "Entity::Null()")
            info.parent = Entity::Null();
        else
            info.parent = m_handleNames.at(parentHandleName);

        #if PRINT_SERIALIZATION_PROCESS
        std::cerr << "  Load Entity\n";
        std::cerr << "    hndl: " << handleName << '\n';
        std::cerr << "    pos:  " << info.position.x << ", " << info.position.y << ", " << info.position.z << '\n';
        std::cerr << "    rot:  " << info.rotation.x << ", " << info.rotation.y << ", " << info.rotation.z << ", " << info.rotation.w << '\n';
        std::cerr << "    scl:  " << info.scale.x << ", " << info.scale.y << ", " << info.scale.z << '\n';
        std::cerr << "    prnt: " << static_cast<EntityTraits::underlying_type>(info.parent) << '\n';
        std::cerr << "    tag:  " << info.tag << '\n';
        std::cerr << "    lyr:  " << layer << '\n';
        std::cerr << "    flag: " << flags << '\n';
        #endif

        Entity entity = m_registry->Add<Entity>(std::move(info));
        m_handleNames.emplace(std::move(handleName), entity);
        return entity;
    }

    void SceneReader::LoadBoxCollider(Entity entity, std::stringstream& args)
    {
        BoxProperties properties;
        bool isTrigger;

        args.ignore(std::numeric_limits<std::streamsize>::max(), '{');
        args >> properties.center.x >> properties.center.y >> properties.center.z;
        args.ignore(std::numeric_limits<std::streamsize>::max(), '{');
        args >> properties.extents.x >> properties.extents.y >> properties.extents.z;
        args.ignore(std::numeric_limits<std::streamsize>::max(), '}');
        args >> isTrigger;

        #if PRINT_SERIALIZATION_PROCESS
        std::cerr << "  LoadBoxCollider\n";
        std::cerr << "    cent: " << properties.center.x << ", " << properties.center.y << ", " << properties.center.z << '\n'
                  << "    ext:  " << properties.extents.x << ", " << properties.extents.y << ", " << properties.extents.z << '\n'
                  << "    trig: " << isTrigger << '\n';
        #endif

        m_registry->Add<Collider>(entity, properties, isTrigger);
    }

    void SceneReader::LoadCapsuleCollider(Entity entity, std::stringstream& args)
    {
        CapsuleProperties properties;
        bool isTrigger;

        args.ignore(std::numeric_limits<std::streamsize>::max(), '{');
        args >> properties.center.x >> properties.center.y >> properties.center.z;

        args.ignore(std::numeric_limits<std::streamsize>::max(), '}');

        args >> properties.height >> properties.radius >> isTrigger;

        #if PRINT_SERIALIZATION_PROCESS
        std::cerr << "  LoadCapsuleCollider\n";
        std::cerr << "    cent: " << properties.center.x << ", " << properties.center.y << ", " << properties.center.z << '\n'
                  << "    heig: " << properties.height << '\n'
                  << "    rad:  " << properties.radius << '\n'
                  << "    trig: " << isTrigger << '\n';
        #endif

        m_registry->Add<Collider>(entity, properties, isTrigger);
    }

    void SceneReader::LoadHullCollider(Entity entity, std::stringstream& args)
    {
        std::cerr << "  LoadHullCollider\n";
        (void)entity;
        (void)args;
    }

    void SceneReader::LoadSphereCollider(Entity entity, std::stringstream& args)
    {

        SphereProperties properties;
        bool isTrigger;

        args.ignore(std::numeric_limits<std::streamsize>::max(), '{');
        args >> properties.center.x >> properties.center.y >> properties.center.z;

        args.ignore(std::numeric_limits<std::streamsize>::max(), '}');

        args >> properties.radius >> isTrigger;

        #if PRINT_SERIALIZATION_PROCESS
        std::cerr << "  LoadSphereCollider\n";
        std::cerr << "    cent: " << properties.center.x << ", " << properties.center.y << ", " << properties.center.z << '\n'
                  << "    rad:  " << properties.radius << '\n'
                  << "    trig: " << isTrigger << '\n';
        #endif

        m_registry->Add<Collider>(entity, properties, isTrigger);
    }

    void SceneReader::LoadPhysicsBody(Entity entity, std::stringstream& args)
    {
        /** @todo need lin/ang freedom */

        PhysicsProperties properties;

        [[maybe_unused]] std::string handleName;

        args >> handleName
             >> properties.mass
             >> properties.drag
             >> properties.angularDrag
             >> properties.restitution
             >> properties.friction
             >> properties.useGravity
             >> properties.isKinematic;
        
        #if PRINT_SERIALIZATION_PROCESS
        std::cerr << "  LoadPhysicsBody\n";
        std::cerr << "    mass: " << properties.mass << '\n'
                  << "    drag: " << properties.drag << '\n'
                  << "    ang:  " << properties.angularDrag << '\n'
                  << "    rest: " << properties.restitution << '\n'
                  << "    fric: " << properties.friction << '\n'
                  << "    grav: " << properties.useGravity << '\n'
                  << "    kin:  " << properties.isKinematic << '\n';
        #endif

        m_registry->Add<PhysicsBody>(entity, properties);
    }

    void SceneReader::LoadPointLight(Entity entity, std::stringstream& args)
    {
        PointLightInfo properties;

        args.ignore(std::numeric_limits<std::streamsize>::max(), '{');
        args >> properties.pos.x >> properties.pos.y >> properties.pos.z;
        args.ignore(std::numeric_limits<std::streamsize>::max(), '{');
        args >> properties.ambient.x >> properties.ambient.y >> properties.ambient.z;
        args.ignore(std::numeric_limits<std::streamsize>::max(), '{');
        args >> properties.diffuseColor.x >> properties.diffuseColor.y >> properties.diffuseColor.z;
        args.ignore(std::numeric_limits<std::streamsize>::max(), '}');

        args >> properties.diffuseIntensity
             >> properties.attConst
             >> properties.attLin
             >> properties.attQuad;

        #if PRINT_SERIALIZATION_PROCESS
        std::cerr << "  Load PointLight\n";
        std::cerr << "    pos: " << properties.pos.x << ", " << properties.pos.y << ", " << properties.pos.z << '\n'
                  << "    amb: " << properties.ambient.x << ", " << properties.ambient.y << ", " << properties.ambient.z << '\n'
                  << "    dif: " << properties.diffuseColor.x << ", " << properties.diffuseColor.y << ", " << properties.diffuseColor.z << '\n'
                  << "    int: " << properties.diffuseIntensity << '\n'
                  << "    cst: " << properties.attConst << '\n'
                  << "    lin: " << properties.attLin << '\n'
                  << "    qua: " << properties.attQuad << '\n';
        #endif

        m_registry->Add<PointLight>(entity, properties);
    }

    void SceneReader::LoadMeshRenderer(Entity entity, std::stringstream& args)
    {
        std::string mesh, techniqueName;
        graphics::Material material;

        args.ignore(std::numeric_limits<std::streamsize>::max(), '\"');
        std::getline(args, mesh, '\"');
        args.ignore(std::numeric_limits<std::streamsize>::max(), '\"');
        std::getline(args, material.baseColor, '\"');
        args.ignore(std::numeric_limits<std::streamsize>::max(), '\"');
        std::getline(args, material.normal, '\"');
        args.ignore(std::numeric_limits<std::streamsize>::max(), '\"');
        std::getline(args, material.roughness, '\"');
        args >> techniqueName;
        auto techniqueType = ToTechniqueType(techniqueName);

        #if PRINT_SERIALIZATION_PROCESS
        std::cerr << "  LoadMeshRenderer\n";
        std::cerr << "    mesh: " << mesh << '\n'
                  << "    base: " << material.baseColor << '\n'
                  << "    norm: " << material.normal << '\n'
                  << "    roug: " << material.roughness << '\n'
                  << "    teq:  " << static_cast<unsigned>(techniqueType) << '\n';
        #endif

        m_registry->Add<MeshRenderer>(entity, std::move(mesh), std::move(material), techniqueType);
    }
}