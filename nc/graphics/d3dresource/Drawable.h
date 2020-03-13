#pragma once
#include <vector>
#include <memory>

namespace DirectX { struct XMMATRIX; }
namespace nc { class Transform; }
namespace nc::graphics
{
    class Graphics;
    class PSMaterialConstants;    
}

namespace nc::graphics::d3dresource
{
    class GraphicsResource;
    class IndexBuffer;

    class Drawable
    {
        template<class T>
        friend class DrawableBase;

        public:
            Drawable() = default;
            virtual ~Drawable() = default;
            Drawable(const Drawable&) = delete;
            Drawable& operator=(const Drawable&) = delete;

            virtual void UpdateTransformationMatrix(Transform* transform) noexcept = 0;
            virtual DirectX::XMMATRIX GetTransformXM() const noexcept = 0;
            virtual PSMaterialConstants* GetMaterial() noexcept = 0;

            void Draw(Graphics& graphics) const noexcept;
        
            template<class T>
            T* QueryBindable() noexcept;

        protected:
            void AddBind(std::unique_ptr<GraphicsResource> bind);
            void AddIndexBuffer(std::unique_ptr<IndexBuffer> ibuf);

        private:
            const IndexBuffer* m_indexBuffer = nullptr;
            std::vector<std::unique_ptr<GraphicsResource>> m_binds;

            virtual const std::vector<std::unique_ptr<GraphicsResource>>& GetStaticBinds() const noexcept = 0;
    };

    template<class T>
    T* Drawable::QueryBindable() noexcept
    {
        for(auto& b : m_binds)
                {
                    if(auto pt = dynamic_cast<T*>(b.get()))
                    {
                        return pt;
                    }
                }
                return nullptr;
    }
}