#include "PostProcessBufferResource.h"
#include "graphics2/frontend/subsystem/PostProcessState.h"

#include "ncutility/NcError.h"

#include <ranges>

namespace nc::graphics
{
PostProcessBufferResource::PostProcessBufferResource(std::vector<PostProcessDataVariable> variables)
    : m_variables{std::move(variables)}
{
}

void PostProcessBufferResource::SetVariable(PostProcessPass::type passId, Diligent::IBuffer& buffer)
{
    GetVariable(passId).Set(&buffer);
}

auto PostProcessBufferResource::GetVariable(PostProcessPass::type passId) -> Diligent::IShaderResourceVariable&
{
    auto pos = std::ranges::find_if(
        m_variables,
        [passId](const auto& variable) {
            return variable.passId == passId;
        }
    );

    NC_ASSERT(pos != m_variables.end(), fmt::format("No variable exists for post process pass '{}'", passId));
    return *pos->variable;
}
} // namespace nc::graphics
