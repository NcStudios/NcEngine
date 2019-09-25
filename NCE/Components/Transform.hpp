#ifndef NCE_COMPONENTS_TRANSFORM
#define NCE_COMPONENTS_TRANSFORM

#include <iostream>

#include "../Common/Common.hpp"
#include "../Common/Vector.hpp"

namespace NCE::Components
{
    class Transform : public NCE::Common::Component
    {
        private:
            NCE::Common::Vector4 m_rect;
        public:            
            Transform(Common::EntityWeakPtr parent_);

            int GetX() const;
            int GetY() const;
            int GetW() const;
            int GetH() const;

            const NCE::Common::Vector4 GetVector4() const;
            const NCE::Common::Rect GetRect() const;

            void Set(int x_, int y_, int w_, int h_);
            void SetPosition(int x_, int y_);
            void SetDimensions(int w_, int h_);

            void Translate(int x_, int y_);
            
    };
}

#endif