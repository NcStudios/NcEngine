#pragma once

#include "ncengine/ecs/Entity.h"

#include "Common/interface/RefCntAutoPtr.hpp"
#include "Graphics/GraphicsEngine/interface/GraphicsTypes.h"
#include "Graphics/GraphicsEngine/interface/PipelineState.h"

#include <vector>
#include <string_view>

namespace nc::graphics
{
struct OrderedMaterialPass
{
    uint32_t materialPassID;
    uint32_t renderOrder;
};

struct Material
{
    std::vector<OrderedMaterialPass> materialPasses;
};

struct MaterialPass
{
    uint32_t id; // @todo: hash? How to ensure uniqueness?
    Diligent::RefCntAutoPtr<Diligent::IPipelineState> pso; // Pass-specific shader and pipeline data
    Diligent::RefCntAutoPtr<Diligent::IPipelineResourceSignature> resourceSignature; // (Optional) Holds the buffer array metadata for this MaterialPass. Each object instance that is rendered via this MaterialPass can add it's instance properties to this array.
    Diligent::RefCntAutoPtr<Diligent::IShaderResourceBinding> resourceBinding; //  (Optional)  Binds the buffer array data for this MaterialPass.
};

class RendererSystem
{
    public:
        RendererSystem(const std::string_view materialPassCachePath);

        /*
            
        */
        void SetMaterial(Entity renderer, const Material& material);

    private:
        /*
            Loads all material passes from the material pass cache.
            Remarks: Load each pass from the cache into a vector of MaterialPass, which is not ordered by render order.
        */
        void LoadMaterialPassCache();

        std::string m_materialPassCachePath;
};
} // namespace nc::graphics
