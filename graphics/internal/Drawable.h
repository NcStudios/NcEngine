#pragma once
#include "Graphics.h"
#include "DirectXMath.h"
#include <vector>
#include <memory>
#include "Bindable.h"

namespace nc::graphics::internal
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

        private:
            const IndexBuffer* m_indexBuffer = nullptr;
            std::vector<std::unique_ptr<Bindable>> m_binds;

            virtual const std::vector<std::unique_ptr<Bindable>>& GetStaticBinds() const noexcept = 0;
    };
}