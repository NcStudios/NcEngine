#pragma once
#include "ConstantBuffer.h"
#include "DirectXMath.h"

namespace nc::graphics { class Model; }

namespace nc::graphics::d3dresource
{
    class TransformCbuf : public GraphicsResource
    {
        public:
            TransformCbuf(Graphics& graphics, const std::string& tag, const Model& parent, UINT slot = 0u);
            void Bind(Graphics& graphics) noexcept override;

            //static std::shared_ptr<GraphicsResource> Resolve()

            static std::shared_ptr<GraphicsResource> AquireUnique(Graphics& graphics, const std::string& tag, const Model& parent, UINT slot)
            {
                return std::make_shared<TransformCbuf>(graphics, tag, parent, slot);
                //return GraphicsResourceManager::Resolve<TransformCbuf>(graphics, tag, parent, slot);
            }

            static std::string GetUID(const std::string& tag, const Model& parent, UINT slot) noexcept
            {
                return typeid(TransformCbuf).name() + tag;
            }

        private:
            struct Transforms
            {
                DirectX::XMMATRIX modelView;
                DirectX::XMMATRIX model;
            };

            static std::unique_ptr<VertexConstantBuffer<Transforms>> m_vcbuf;
            const Model& m_parent;
    };
}