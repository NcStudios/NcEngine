#include "Vector.hpp"

namespace NCE::Common
{
    void Vector2::Set(int t_x, int t_y)
    {
        _x = t_x;
        _y = t_y;
    }

    void Vector4::Set(double t_a, double t_b, double t_c, double t_d)
    {
        _a = t_a;
        _b = t_b;
        _c = t_c;
        _d = t_d;
    }

}