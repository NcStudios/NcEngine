/**
 * @file UIImage.h
 * @copyright Jaremie Romer and McCallister Romer 2025
 */
#pragma once

#include "ncengine/asset/AssetViews.h"
#include "ncengine/utility/Signal.h"
#include "imgui.h"

namespace nc::ui
{
using UIImageProvider = std::function<ImTextureID(uint32_t)>;
void SetUIImageProvider(UIImageProvider provider);

/**
 * @brief A wrapper for ImGui::DrawImage() that draws an asset::TextureView.
 */
class UIImage
{
    public:
        /**
         * @brief Construct a new UIImage object.
         */
        UIImage(asset::TextureView texture, Vector2 dimensions);
};
} //end namespace nc::ui
