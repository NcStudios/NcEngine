#pragma once

#include "ncengine/asset/AssetLoader.h"

#include <span>

namespace nc::physics
{
class ShapeFactory;

class JoltAssetLoader : public asset::PhysicsAssetLoader
{
    public:
        explicit JoltAssetLoader(ShapeFactory& factory);

        auto ConvertToConvexHull(const asset::NamedMesh& mesh) const -> asset::ConvexHull override;

    private:
        ShapeFactory* m_shapeFactory;
};
} // namespace nc::physics
