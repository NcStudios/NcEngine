#include "ui/UIImage.h"

namespace nc::ui
{
static UIImageProvider g_uiImageProvider;

void SetUIImageProvider(UIImageProvider provider) {
    g_uiImageProvider = std::move(provider);
}

UIImage::UIImage(asset::TextureView texture, Vector2 dimensions)
{
    auto id = g_uiImageProvider(texture.index);
    ImGui::Image(id, ImVec2(dimensions.x, dimensions.y));
}
} //end namespace nc::ui
