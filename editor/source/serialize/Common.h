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
    const auto GeneratedSourceExtension = std::string{".gen"};

    const auto AddEntitySceneAction = std::string{"NC_SCENE_ACTION_ADD_ENTITY"};
    const auto AddBoxColliderSceneAction = std::string{"NC_SCENE_ACTION_ADD_BOX_COLLIDER"};
    const auto AddCapsuleColliderSceneAction = std::string{"NC_SCENE_ACTION_ADD_CAPSULE_COLLIDER"};
    const auto AddHullColliderSceneAction = std::string{"NC_SCENE_ACTION_ADD_HULL_COLLIDER"};
    const auto AddSphereColliderSceneAction = std::string{"NC_SCENE_ACTION_ADD_SPHERE_COLLIDER"};
    const auto AddConcaveColliderSceneAction = std::string{"NC_SCENE_ACTION_ADD_CONCAVE_COLLIDER"};
    const auto AddPointLightSceneAction = std::string{"NC_SCENE_ACTION_ADD_POINT_LIGHT"};
    const auto AddPhysicsBodySceneAction = std::string{"NC_SCENE_ACTION_ADD_PHYSICS_BODY"};
    const auto AddMeshRendererSceneAction = std::string{"NC_SCENE_ACTION_ADD_MESH_RENDERER"};

    inline std::string StripCharacter(std::string value, char target)
    {
        value.erase(std::remove(value.begin(), value.end(), target), value.end());
        return value;
    }
}