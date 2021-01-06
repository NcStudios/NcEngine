#include "Step.h"
#include "graphics/d3dresource/GraphicsResource.h"
#include "graphics/Graphics.h"
#include "graphics/Model.h"
#include "FrameManager.h"

namespace nc::graphics
{
    Step::Step(size_t targetPass)
    : targetPass{targetPass}
    {}
}