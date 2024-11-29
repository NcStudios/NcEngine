#pragma once

#include "graphics2/ShaderTypes.h"

#include "ncengine/graphics/PostProcess.h"

#include "Graphics/GraphicsEngine/interface/Buffer.h"
#include "Graphics/GraphicsEngine/interface/ShaderResourceVariable.h"

namespace nc::graphics
{
struct PostProcessState;

// 
struct PostProcessDataVariable
{
    Diligent::IShaderResourceVariable* variable = nullptr;
    PostProcessPass::type passId = PostProcessPass::None;
};

// 
class PostProcessBufferResource
{
    public:
        explicit PostProcessBufferResource(std::vector<PostProcessDataVariable> variables);

        void SetVariable(PostProcessPass::type passId, Diligent::IBuffer& buffer);
        auto GetVariable(PostProcessPass::type passId) -> Diligent::IShaderResourceVariable&;

    private:
        std::vector<PostProcessDataVariable> m_variables;
};
} // namespace nc::graphics
