#pragma once

#include "ncengine/asset/NcAsset.h"
#include "ncengine/ecs/Ecs.h"

#include <memory>

namespace nc
{
class NcEngine;
}

namespace nc::ui::editor
{
class Editor
{
    public:
        virtual ~Editor() = default;
        virtual void Draw(ecs::Ecs world, asset::NcAsset& assetModule) = 0;
};

auto BuildEditor(NcEngine* engine) -> std::unique_ptr<Editor>;
} // namespace nc::ui::editor
