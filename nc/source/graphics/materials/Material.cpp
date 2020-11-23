#include "Material.h"
#include "graphics/d3dresource/GraphicsResourceManager.h"
#include "graphics/Model.h"
#include "Engine.h"
#include "config/Config.h"


namespace nc::graphics
{
    void Material::Submit(FrameManager& frame, const Model& model) noexcept
    {
        for (const auto& technique : m_techniques)
        {
            technique.Submit(frame, model);
        }
    }

    void Material::AddTechnique(const Technique& technique) noexcept
    {
        m_techniques.push_back(std::move(technique));
    }
}