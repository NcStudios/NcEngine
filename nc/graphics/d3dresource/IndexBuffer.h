#pragma once
#include "GraphicsResource.h"
#include <stdint.h>

#include <iostream>

namespace nc::graphics::d3dresource
{
    class IndexBuffer : public GraphicsResource
    {
        public:
            IndexBuffer(Graphics& graphics, const std::vector<uint16_t>& indices, std::string& tag);
            void Bind(Graphics& graphics) noexcept override;
            UINT GetCount() const noexcept;

            static std::shared_ptr<GraphicsResource> Aquire(Graphics& graphics, const std::vector<uint16_t>& indices, std::string& tag)
            {
                std::cout << "IndexBuffer::Aquire()" << std::endl;
                std::cout << "indices size: " << indices.size() << std::endl;
                if(indices.data() == nullptr) { std::cout << "nullptr" << std::endl; }

                return GraphicsResourceManager::Aquire<IndexBuffer>(graphics, indices, tag);
            }

            static std::string GetUID(const std::vector<uint16_t>& indices, std::string& tag) noexcept
            {
                return typeid(IndexBuffer).name() + tag;
            }
        
        protected:
            UINT m_count;
            Microsoft::WRL::ComPtr<ID3D11Buffer> m_indexBuffer;
            const std::string m_tag;
    };

} //end namespace nc::graphics::internal