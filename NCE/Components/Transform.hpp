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
            NCE::Common::Vector4 _rect;
        public:            
            Transform(Common::EntityWeakPtr t_parent);

            int GetX() const;
            int GetY() const;
            int GetW() const;
            int GetH() const;

            const NCE::Common::Vector4 GetVector4() const;
            const NCE::Common::Rect GetRect() const;

            void Set(int t_x, int t_y, int t_w, int t_h);
            void SetPosition(int t_x, int t_y);
            void SetDimensions(int t_w, int t_h);

            void Translate(int t_x, int t_y);
            
    };
}

#endif