#pragma once
#include "Drawable.h"
#include "IndexBuffer.h"

namespace nc::graphics::d3dresource
{
    template<class T>
    class DrawableBase : public Drawable
    {
        protected:
            bool IsStaticInitialized() const noexcept
            {
                return !staticBinds.empty();
            }

            void AddStaticBind(std::unique_ptr<Bindable> bind)
            {
                staticBinds.push_back(std::move(bind));
            }

            void AddStaticIndexBuffer(std::unique_ptr<IndexBuffer> iBuf)
            {
                m_indexBuffer = iBuf.get();
                staticBinds.push_back(std::move(iBuf));
            }

            void SetIndexFromStatic()
            {
                for(const auto& b : staticBinds)
                {
                    if(const auto p = dynamic_cast<IndexBuffer*>(b.get()))
                    {
                        m_indexBuffer = p;
                        return;
                    }
                }
                //what if it doesn't find?
            }

        private:
            static std::vector<std::unique_ptr<Bindable>> staticBinds;

            const std::vector<std::unique_ptr<Bindable>>& GetStaticBinds() const noexcept
            {
                return staticBinds;
            }
    };

    template<class T>
    std::vector<std::unique_ptr<Bindable>> DrawableBase<T>::staticBinds;
}