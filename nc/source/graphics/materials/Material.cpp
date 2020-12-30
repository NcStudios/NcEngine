#include "Material.h"
#include "graphics/d3dresource/GraphicsResourceManager.h"
#include "graphics/Model.h"
#include "graphics/techniques/TechniqueManager.h"
#include "Engine.h"
#include "config/Config.h"

namespace nc::graphics
{
    Material::Material() = default;
    Material::~Material() = default;

    void Material::Submit(FrameManager& frame, const Model& model) const noexcept
    {
        m_technique->Submit(frame, model);
    }
}