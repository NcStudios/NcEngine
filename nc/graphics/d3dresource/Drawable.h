#pragma once
#include "Graphics.h"
#include "DirectXMath.h"
#include <vector>
#include <memory>
#include "Bindable.h"

namespace nc::graphics::d3dresource
{
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

            virtual DirectX::XMMATRIX GetTransformXM() const noexcept = 0;
            void Draw(Graphics& graphics) const noexcept;
            virtual void Update(float dt) noexcept = 0;
        
        protected:
            void AddBind(std::unique_ptr<Bindable> bind);
            void AddIndexBuffer(std::unique_ptr<IndexBuffer> ibuf);

            template<class T>
            T* QueryBindable() noexcept
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

        private:
            const IndexBuffer* m_indexBuffer = nullptr;
            std::vector<std::unique_ptr<Bindable>> m_binds;

            virtual const std::vector<std::unique_ptr<Bindable>>& GetStaticBinds() const noexcept = 0;
    };
}