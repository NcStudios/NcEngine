#include "Pass.h"

#include "ncutility/NcError.h"

namespace nc::graphics
{
Pass::Pass(Diligent::RefCntAutoPtr<Diligent::IPipelineState> pso_,
           Sinks sinks_,
           Sources sources_)
    : pso{std::move(pso_)},
      sinks{std::move(sinks_)},
      sources{std::move(sources_)}
{
}
}