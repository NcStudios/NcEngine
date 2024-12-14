#pragma once

#include "ncengine/graphics/PostProcess.h"

#include "Graphics/GraphicsEngine/interface/Buffer.h"
#include "Graphics/GraphicsEngine/interface/ShaderResourceVariable.h"

#include <vector>

namespace nc::graphics
{
struct PostProcessState;

// Shader resource variable tied to its associated post process pass.
struct PostProcessDataVariable
{
    Diligent::IShaderResourceVariable* variable = nullptr;
    PostProcessPassFlag::type passId = PostProcessPassFlag::None;
};

// Set of uniform buffer variables for post process property types. The actual buffers are owned by pass instances.
class PostProcessPropertyBufferResource
{
    public:
        explicit PostProcessPropertyBufferResource(std::vector<PostProcessDataVariable> variables);

        void SetVariable(PostProcessPassFlag::type passId, Diligent::IBuffer& buffer);
        auto GetVariable(PostProcessPassFlag::type passId) -> Diligent::IShaderResourceVariable&;

    private:
        std::vector<PostProcessDataVariable> m_variables;
};
} // namespace nc::graphics
