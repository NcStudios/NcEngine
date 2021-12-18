#pragma once

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <string>
#include <unordered_map>

namespace nc::editor
{
    struct FileExtension
    {
        static constexpr auto Header    = std::string_view{".h"};
        static constexpr auto Source    = std::string_view{".cpp"};
        static constexpr auto Generated = std::string_view{".inl"};
    };

    struct SceneMacro
    {
        static constexpr auto Begin                 = std::string_view{"( "};
        static constexpr auto End                   = std::string_view{" )"};
        static constexpr auto Separator             = std::string_view{" , "};
        static constexpr auto AddEntity             = std::string_view{"NC_EDITOR_ADD_ENTITY"};
        static constexpr auto AddCamera             = std::string_view{"NC_EDITOR_ADD_CAMERA"};
        static constexpr auto AddBoxCollider        = std::string_view{"NC_EDITOR_ADD_BOX_COLLIDER"};
        static constexpr auto AddCapsuleCollider    = std::string_view{"NC_EDITOR_ADD_CAPSULE_COLLIDER"};
        static constexpr auto AddHullCollider       = std::string_view{"NC_EDITOR_ADD_HULL_COLLIDER"};
        static constexpr auto AddSphereCollider     = std::string_view{"NC_EDITOR_ADD_SPHERE_COLLIDER"};
        static constexpr auto AddConcaveCollider    = std::string_view{"NC_EDITOR_ADD_CONCAVE_COLLIDER"};
        static constexpr auto AddPointLight         = std::string_view{"NC_EDITOR_ADD_POINT_LIGHT"};
        static constexpr auto AddPhysicsBody        = std::string_view{"NC_EDITOR_ADD_PHYSICS_BODY"};
        static constexpr auto AddMeshRenderer       = std::string_view{"NC_EDITOR_ADD_MESH_RENDERER"};
        static constexpr auto BoxProperties         = std::string_view{"NC_EDITOR_BOX_PROPERTIES"};
        static constexpr auto CapsuleProperties     = std::string_view{"NC_EDITOR_CAPSULE_PROPERTIES"};
        static constexpr auto EntityInfo            = std::string_view{"NC_EDITOR_ENTITY_INFO"};
        static constexpr auto HullProperties        = std::string_view{"NC_EDITOR_HULL_PROPERTIES"};
        static constexpr auto Material              = std::string_view{"NC_EDITOR_MATERIAL"};
        static constexpr auto PhysicsProperties     = std::string_view{"NC_EDITOR_PHYSICS_PROPERTIES"};
        static constexpr auto PointLightInfo        = std::string_view{"NC_EDITOR_POINT_LIGHT_INFO"};
        static constexpr auto RegisterAudioListener = std::string_view{"NC_EDITOR_REGISTER_AUDIO_LISTENER"};
        static constexpr auto SetCamera             = std::string_view{"NC_EDITOR_SET_CAMERA"};
        static constexpr auto SphereProperties      = std::string_view{"NC_EDITOR_SPHERE_PROPERTIES"};
    };

    inline std::string StripCharacter(std::string value, char target)
    {
        value.erase(std::remove(value.begin(), value.end(), target), value.end());
        return value;
    }
}