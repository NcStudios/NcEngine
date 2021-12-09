#pragma once

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <string>
#include <unordered_map>

namespace nc::editor
{
    const auto HeaderExtension = std::string{".h"};
    const auto SourceExtension = std::string{".cpp"};
    const auto GeneratedSourceExtension = std::string{".inl"};

    const auto AddEntitySceneAction             = std::string{"NC_EDITOR_ADD_ENTITY"};
    const auto AddCameraSceneAction             = std::string{"NC_EDITOR_ADD_CAMERA"};
    const auto AddBoxColliderSceneAction        = std::string{"NC_EDITOR_ADD_BOX_COLLIDER"};
    const auto AddCapsuleColliderSceneAction    = std::string{"NC_EDITOR_ADD_CAPSULE_COLLIDER"};
    const auto AddHullColliderSceneAction       = std::string{"NC_EDITOR_ADD_HULL_COLLIDER"};
    const auto AddSphereColliderSceneAction     = std::string{"NC_EDITOR_ADD_SPHERE_COLLIDER"};
    const auto AddConcaveColliderSceneAction    = std::string{"NC_EDITOR_ADD_CONCAVE_COLLIDER"};
    const auto AddPointLightSceneAction         = std::string{"NC_EDITOR_ADD_POINT_LIGHT"};
    const auto AddPhysicsBodySceneAction        = std::string{"NC_EDITOR_ADD_PHYSICS_BODY"};
    const auto AddMeshRendererSceneAction       = std::string{"NC_EDITOR_ADD_MESH_RENDERER"};
    const auto SetCameraSceneAction             = std::string{"NC_EDITOR_SET_CAMERA"};
    const auto RegisterAudioListenerSceneAction = std::string{"NC_EDITOR_REGISTER_AUDIO_LISTENER"};

    inline std::string StripCharacter(std::string value, char target)
    {
        value.erase(std::remove(value.begin(), value.end(), target), value.end());
        return value;
    }
}