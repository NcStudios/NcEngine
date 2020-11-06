#include "Prefabs.h"
#include "graphics/Graphics.h"
#include "graphics/Model.h"
#include "graphics/Mesh.h"
#include "component/Renderer.h"

using namespace nc;

namespace prefabs
{
    nc::EntityHandle CreateCoalPiece(const nc::Vector3& position, const nc::Vector3& rotation, const nc::Vector3& scale, const std::string tag)
    {
        auto coalMaterial = graphics::PBRMaterial{{"project//Textures//CoalPiece_Material_BaseColor.png", "project//Textures//CoalPiece_Material_Normal.png",  "project//Textures//CoalPiece_Material_Roughness.png", "nc//source//graphics//DefaultTexture.png"}};
        auto coalHandle = NcCreateEntity(position, rotation, scale, tag);
        auto coalMesh = graphics::Mesh{"project//Models//CoalPiece.fbx"};
        NcAddEngineComponent<Renderer>(coalHandle, coalMesh, coalMaterial);
        return coalHandle;
    }

    nc::EntityHandle CreateRubyPiece(const nc::Vector3& position, const nc::Vector3& rotation, const nc::Vector3& scale, const std::string tag)
    {
        auto rubyMaterial = graphics::PBRMaterial{{"project//Textures//RubyPiece_Material_BaseColor.png", "project//Textures//RubyPiece_Material_Normal.png",  "project//Textures//RubyPiece_Material_Roughness.png", "nc//source//graphics//DefaultTexture.png"}};
        auto rubyHandle = NcCreateEntity(position, rotation, scale, tag);
        auto rubyMesh = graphics::Mesh{"project//Models//RubyPiece.fbx"};
        NcAddEngineComponent<Renderer>(rubyHandle, rubyMesh, rubyMaterial);
        return rubyHandle;
    }
}