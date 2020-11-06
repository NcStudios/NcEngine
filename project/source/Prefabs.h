#pragma once
#include <NcCommon.h>

namespace prefabs
{
    nc::EntityHandle CreateCoalPiece(const nc::Vector3& position, const nc::Vector3& rotation, const nc::Vector3& scale, const std::string tag);
    nc::EntityHandle CreateRubyPiece(const nc::Vector3& position, const nc::Vector3& rotation, const nc::Vector3& scale, const std::string tag);
}