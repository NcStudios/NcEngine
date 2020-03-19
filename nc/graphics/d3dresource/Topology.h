#pragma once
#include "GraphicsResource.h"

namespace nc::graphics::d3dresource
{
    class Topology : public GraphicsResource
    {
        public:
            Topology(Graphics& graphics,D3D11_PRIMITIVE_TOPOLOGY type);
            void Bind(Graphics& graphics) noexcept override;

            static std::shared_ptr<GraphicsResource> Aquire(Graphics& graphics, D3D11_PRIMITIVE_TOPOLOGY type)
            {
                return GraphicsResourceManager::Aquire<Topology>(graphics, type);
            }

            static std::string GetUID(D3D11_PRIMITIVE_TOPOLOGY topology) noexcept
            {
                return typeid(Topology).name() + std::to_string(topology);
            }

        protected:
            D3D11_PRIMITIVE_TOPOLOGY m_type;
    };
}