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
    std::string_view name; // Used to add a Material Pass to a Material. Maps 1-1 with PSO name.
    uint32_t id; // @todo: hash? How to ensure uniqueness?
    Diligent::RefCntAutoPtr<Diligent::IPipelineState> pso; // Pass-specific shader and pipeline data
    Diligent::RefCntAutoPtr<Diligent::IPipelineResourceSignature> resourceSignature; // (Optional) Holds the buffer array metadata for this MaterialPass. Each object instance that is rendered via this MaterialPass can add it's instance properties to this array.
    Diligent::RefCntAutoPtr<Diligent::IShaderResourceBinding> resourceBinding; //  (Optional)  Binds the buffer array data for this MaterialPass.
};

struct PassRenderers
{
    uint32_t materialPassID;
    std::vector<Entity> renderers;
};

class RendererSystem
{
    public:
        RendererSystem(const std::string_view materialPassCachePath);

        /* Entry point from scene/client usage into the RendererSystem. Will want to expose route to this in ncengine/include (or move entirely).
           MaterialPass objects can belong to one or more Materials (think Shadow pass). We don't want to group renderers by material, but rather by pass.
           This method populates the "render order" buckets with the pass ID and the Entity ID, ending in a data structure like this:
           RENDER ORDER 0:
                ShadowPass:
                    Entity 0x001, 0x006, 0x011, 0x124
           RENDER ORDER 1:
                ToonPass:
                    Entity 0x001, 0x006, 0x007, 0x008, 0x009
                PbrPass:
                    Entity 0x011, 0x124
            RENDER ORDER 2:
                OutlinePass:
                    Entity 0x001, 0x006, 0x007, 0x008, 0x009
        */
        void SetMaterial(Entity renderer, const Material& material, uint32_t materialInstanceID = 0u);

        /* Used in the scene to construct a material. Should also be in ncengine/include.
           We will know the Pass Name at design time, so we can expose some kind of material creation interface like this:
           TestScene:

           auto toonMaterial = graphics::Material
           {
              OrderedMaterialPass{0, MaterialPassId("Shadow Pass")},
              OrderedMaterialPass{1, MaterialPassId("Toon Pass")},
              OrderedMaterialPass{2, MaterialPassId("Outline Pass")}
           };

           auto skellyMesh = world.Emplace<graphics::MeshRenderer>(skeleton, "skeleton.nca");
           skellyMesh.SetMaterial(toonMaterial);
        */
        auto MaterialPassId(std::string_view materialPassName) -> uint32_t;

    private:
        /*  Load each pass from the cache into a vector of MaterialPass (m_loadedPasses), which is not ordered by render order. (No way to set it in the DRSN, as far as I can see.) */
        void LoadMaterialPassCache();

        std::vector<MaterialPass> m_loadedPasses;
        std::vector<PassRenderers> m_orderedPassRegistry;
        std::string m_materialPassCachePath;
};
} // namespace nc::graphics
